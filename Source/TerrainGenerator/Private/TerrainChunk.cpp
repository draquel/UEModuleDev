#include "TerrainChunk.h"

#include "LevelEditorViewport.h"
#include "Components/DecalComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "MeshData/MeshDataGenerator.h"
#include "Noise.h"
#include "TerrainChunkSettings.h"
#include "TerrainGenerator.h"
#include "TextureUtils.h"
#include "Thread/NoiseTextureThread.h"
#include "Thread/PoissonGeneratorThread.h"

ATerrainChunk::ATerrainChunk()
{
	PrimaryActorTick.bCanEverTick = true;

	Coord = FVector2D();
	RootComponent = CreateDefaultSubobject<USceneComponent>("Chunk Root");
	RootComponent->SetCanEverAffectNavigation(true);
	
	TerrainMesh = CreateDefaultSubobject<UProceduralMeshComponent>("Terrain Mesh");
	TerrainMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TerrainMesh->SetCanEverAffectNavigation(false);
	TerrainMesh->SetupAttachment(GetRootComponent());

	WaterMesh = CreateDefaultSubobject<UProceduralMeshComponent>("Water Mesh");
	WaterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WaterMesh->SetCanEverAffectNavigation(false);
	WaterMesh->SetupAttachment(GetRootComponent());

	WaterCausticsDecal = CreateDefaultSubobject<UDecalComponent>(FName("Water Caustics"));
	WaterCausticsDecal->SetCanEverAffectNavigation(false);
	WaterCausticsDecal->SetupAttachment(GetRootComponent());

	// WaterPostProcessing = CreateDefaultSubobject<UPostProcessComponent>(FName("Water Post Processing"));
	// WaterPostProcessing->SetupAttachment(GetRootComponent());
}

void ATerrainChunk::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void ATerrainChunk::BeginPlay()
{
	Super::BeginPlay();
}

void ATerrainChunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATerrainChunk::Init()
{
	Init(FVector2D(),Settings);
}

void ATerrainChunk::Init(FVector2D chunkCoord, FTerrainChunkSettings settings)
{
	SetPosition(chunkCoord,settings.Size.X);
	Settings = settings;
	SetActorLabel(FString::Printf(TEXT("(%lld,%lld)"),FMath::RoundToInt(chunkCoord.X),FMath::RoundToInt(chunkCoord.Y)),false);

	QTree = UQuadTree(position,Settings.Size,Settings.QuadTreeSettings);
	
	TerrainMesh->SetMaterial(0,Settings.Material);
	WaterMesh->SetMaterial(0,Settings.WaterMaterial);
	WaterCausticsDecal->SetVisibility(false);

	if(settings.WaterCausticsMaterial != nullptr) {
		WaterCausticsDecal->SetMaterial(0,Settings.WaterCausticsMaterial);
		WaterCausticsDecal->AddLocalOffset(FVector(Settings.Size.X/2.0f,Settings.Size.Y/2.0f,-Settings.Size.Z*0.25f));
		WaterCausticsDecal->DecalSize = FVector(Settings.Size.X*0.5f,Settings.Size.Y*0.5f,Settings.Size.Z*0.25f);
		WaterCausticsDecal->SetVisibility(Settings.EnableWater);
	}

	for (int i = 0; i < Settings.FoliageGroups.Num(); i++){ FoliageGroups.Add(FFoliageGroupData()); }
}

void ATerrainChunk::Update(FVector playerPos,bool allowThreads)
{
	if (NoiseTexture == nullptr) {
		GenerateNoise(playerPos,allowThreads);
		return;
	}
	CreateTerrainMesh(playerPos,allowThreads);
	if(Settings.EnableWater && NoiseMinMax.min < 0){ if(!hasWater){ CreateWaterMesh(); } } else { WaterMesh->ClearMeshSection(0); WaterCausticsDecal->SetVisibility(false); hasWater = false; }
	if(Settings.EnableFoliage) { CreateFoliage(playerPos, allowThreads); }
	DebugDraw(0.75f,FColor::Orange);
}

void ATerrainChunk::Disable()
{
	DebugDraw(0.75f,FColor::Cyan);
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	SetActorEnableCollision(false);
	WaterMesh->Deactivate();
	TerrainMesh->Deactivate();
	for (int i = 0; i < FoliageGroups.Num(); i++) {
		for(UInstancedStaticMeshComponent* ism : FoliageGroups[i].FoliageInstances) {
			ism->Deactivate();
		}
	}
}

void ATerrainChunk::Enable()
{
	DebugDraw(0.75f,FColor::Yellow);
	SetActorHiddenInGame(false);
	SetActorTickEnabled(true);
	SetActorEnableCollision(true);
	WaterMesh->Activate();
	TerrainMesh->Activate();
	for (int i = 0; i < FoliageGroups.Num(); i++) {
		for(UInstancedStaticMeshComponent* ism : FoliageGroups[i].FoliageInstances) {
			ism->Activate();
		}
	}
}

// CREATE COMPONENTS

void ATerrainChunk::GenerateNoise(FVector playerPos,bool allowThread)
{
	FVector pos = GetActorLocation();
	UNoise::GenerateMap2D(FIntVector(pos.X-1,pos.Y-1,pos.Z),FIntVector2(Settings.Size.X+Settings.NoiseStepSize*2,Settings.Size.Y+Settings.NoiseStepSize*2),Settings.NoiseStepSize,Settings.NoiseSettings,[this,playerPos,allowThread](FNoiseMap2d* NoiseMap)	{
		NoiseTexture = UNoise::GenerateDataTexture(NoiseMap,FName(FString::Printf(TEXT("Chunk (%lld,%lld) Noise"),FMath::FloorToInt(Coord.X),FMath::FloorToInt(Coord.Y))));
		NoiseMinMax = NoiseMap->MinMax;
		CreateTerrainMesh(playerPos,allowThread);
		if(Settings.EnableWater && NoiseMinMax.min < 0){ if(!hasWater){ CreateWaterMesh(allowThread); } WaterCausticsDecal->SetVisibility(true); } else { WaterMesh->ClearMeshSection(0); WaterCausticsDecal->SetVisibility(false); hasWater = false; }
		if(Settings.EnableFoliage) { CreateFoliage(playerPos, allowThread); }
		DebugDraw(0.75f,FColor::Orange);	
	});
}

void ATerrainChunk::CreateTerrainMesh(FVector playerPos = FVector(),bool allowThread)
{
	if(Settings.QuadtreeMeshEnabled){ CreateQuadtreeMesh(playerPos,allowThread);}
	else { CreateRectMesh(allowThread); }
}

void ATerrainChunk::CreateQuadtreeMesh(FVector playerPos = FVector(), bool allowThread)
{
	QTree.GenerateTree(playerPos);
	if(GetWorld()->IsGameWorld() && allowThread) {
		QuadTreeMeshDataThread = new FQuadTreeMeshDataThread(TerrainMesh,&QTree,Settings.UVScale,Settings.DepthFilter,0,true,NoiseTexture,Settings.Size.Z);
	} else {
		// double start = FPlatformTime::Seconds();
		FMeshData MeshData = UMeshDataGenerator::QuadTreeMesh(NoiseTexture,&QTree,Settings.NoiseStepSize,Settings.UVScale,Settings.DepthFilter,Settings.Size.Z);
		MeshData.CreateProceduralMesh(TerrainMesh);
		MeshData.ConfigureForNavigation(TerrainMesh);

		// double end = FPlatformTime::Seconds();
		// UE_LOG(TerrainGenerator,VeryVerbose,TEXT("TerrainChunk %s::CreateQuadMesh() => Duration: %f s"),*GetActorLabel(),end-start);
	}
}

void ATerrainChunk::CreateRectMesh(bool allowThread)
{
	if(GetWorld()->IsGameWorld() && allowThread && false) {
		// RectMeshDataThread = new FRectMeshDataThread(TerrainMesh,position,Settings.Size,FVector2D(Settings.MeshResolution,Settings.MeshResolution),Settings.WaterUVScale,0,true,&Settings.NoiseSettings,Settings.Size.Z);
	} else {
		// double start = FPlatformTime::Seconds();
		FMeshData MeshData = UMeshDataGenerator::RectMesh(NoiseTexture,position,Settings.Size,Settings.NoiseStepSize,400,Settings.UVScale,Settings.Size.Z);
		MeshData.CreateProceduralMesh(TerrainMesh);
		MeshData.ConfigureForNavigation(TerrainMesh);
		
		// double end = FPlatformTime::Seconds();
		// UE_LOG(TerrainGenerator,VeryVerbose,TEXT("TerrainChunk %s::CreateWaterMesh() => Duration: %f s"),*GetActorLabel(),end-start);
	}
}

void ATerrainChunk::CreateWaterMesh(bool allowThread)
{
	int resolution = Settings.WaterResolution;
	if(GetWorld()->IsGameWorld() && allowThread) {
		WaterMeshDataThread = new FRectMeshDataThread(WaterMesh,position,Settings.Size,FVector2D(resolution,resolution),Settings.WaterUVScale,0,false);
	} else {
		// double start = FPlatformTime::Seconds();
	
		FMeshData MeshData = UMeshDataGenerator::RectMesh(position,Settings.Size,FVector2D(resolution,resolution),Settings.WaterUVScale,nullptr,0);
		MeshData.CreateProceduralMesh(WaterMesh,0,false);

		hasWater = true;
		
		// double end = FPlatformTime::Seconds();
		// UE_LOG(TerrainGenerator,VeryVerbose,TEXT("TerrainChunk %s::CreateWaterMesh() => Duration: %f s"),*GetActorLabel(),end-start);
	}
}
 
void ATerrainChunk::CreateFoliage(FVector playerPos, bool allowThread, bool regenerate)
{
	if (Settings.FoliageGroups.Num() == 0) {
		UE_LOG(TerrainGenerator, Warning, TEXT("FoliageGroups List is empty."));
		return;
	}
	double start = FPlatformTime::Seconds();

	for(int i = 0; i < Settings.FoliageGroups.Num(); i++) {
		if(regenerate || FoliageGroups[i].hasFoliage && Settings.FoliageGroups[i].LoadRange > 0 && FVector2d::Distance(FVector2d(position.X,position.Y),FVector2d(playerPos.X,playerPos.Y)) > Settings.Size.X*Settings.FoliageGroups[i].LoadRange) {
			for(int j = 0; j < FoliageGroups[i].FoliageInstances.Num(); j++){
				FoliageGroups[i].FoliageInstances[j]->ClearInstances();
				FoliageGroups[i].hasFoliage = false;
			}
			if (!regenerate) { continue; }
		}
		if(!FoliageGroups[i].hasFoliage && (Settings.FoliageGroups[i].LoadRange <= 0 || FVector2d::Distance(FVector2d(position.X,position.Y),FVector2d(playerPos.X,playerPos.Y)) <= Settings.Size.X*Settings.FoliageGroups[i].LoadRange)){
			if(FoliageGroups[i].FoliagePoints.Num() == 0) {
				if(GetWorld()->IsGameWorld() && allowThread) {
					PoissonThread = new FPoissonGeneratorThread(this,&Settings.FoliageGroups[i],&FoliageGroups[i],FVector2d(0,0),FVector2d(Settings.Size.X,Settings.Size.Y),Settings.FoliageGroups[i].FoliageMinDist,Settings.FoliageGroups[i].FoliageNewPoints);
				} else {
					FoliageGroups[i].FoliagePoints = UNoise::PoissonDiscSample(FVector2d(0,0),FVector2d(Settings.Size.X,Settings.Size.Y),Settings.FoliageGroups[i].FoliageMinDist,Settings.FoliageGroups[i].FoliageNewPoints);
					InstanceFoliage(&FoliageGroups[i],&Settings.FoliageGroups[i]);
					FoliageGroups[i].hasFoliage = true;	
				}
			} else {
				InstanceFoliage(&FoliageGroups[i],&Settings.FoliageGroups[i]);
				FoliageGroups[i].hasFoliage = true;	
			}
		}
	}	
	
	int totalInstances = 0;
	int totalMeshes = 0;
	for (int i = 0; i < FoliageGroups.Num(); i++) {
		totalInstances += FoliageGroups[i].FoliagePoints.Num();
		totalMeshes += Settings.FoliageGroups[i].Foliage.Num();
	}
	double end = FPlatformTime::Seconds();
	// UE_LOG(TerrainGenerator,Log,TEXT("TerrainChunk::CreateFoliage() => Total Instances: %d, Total Meshes: %d Duration: %f s"),totalInstances,totalMeshes,end-start);	
}

void ATerrainChunk::InstanceFoliage(TArray<UInstancedStaticMeshComponent*>* StaticMeshComponents, TArray<UStaticMesh*>* Foliage, TArray<FVector2D>* Points, FVector Offset, FVector Scale, FMinMax ElevationMinMax, FMinMax SlopeMinMax, bool Collision = false)
{
	for(int i = 0; i < Foliage->Num(); i++)	{
		UInstancedStaticMeshComponent* newISM = (UInstancedStaticMeshComponent*)AddComponentByClass(UInstancedStaticMeshComponent::StaticClass(),true,FTransform(),false);
		newISM->SetStaticMesh(Foliage->operator[](i));
		newISM->AttachToComponent(GetRootComponent(),FAttachmentTransformRules::KeepWorldTransform);
		newISM->SetCollisionEnabled(Collision ? ECollisionEnabled::Type::QueryAndPhysics : ECollisionEnabled::Type::NoCollision);
		StaticMeshComponents->Add(newISM);
	}
	for (int i = 0 ; i < Points->Num(); i++) {
		int32 RandomIndex = FMath::RandRange(0, Foliage->Num() - 1);

		FTerrainEvaluationData TEData;
		EvaluateTerrain(NoiseTexture,FVector(Points->operator[](i).X,Points->operator[](i).Y,0),Settings.Size.Z,ElevationMinMax.ToVector2D(),SlopeMinMax.ToVector2D(),&TEData);
		if(TEData.valid) {
			FTransform InstanceTransform(FRotator(0,FMath::RandRange(-180.0f,180.0f),0),FVector(Points->operator[](i).X+position.X,Points->operator[](i).Y+position.Y,TEData.height)+Offset,Scale);
			StaticMeshComponents->operator[](RandomIndex)->AddInstance(InstanceTransform,true);
		}
	}	
	// UE_LOG(TerrainGenerator,Log,TEXT("TerrainChunk::InstanceFoliage() => Foliage Count: %i, Points Count: %i"),Foliage->Num(),Points->Num());	
}

void ATerrainChunk::InstanceFoliage(FFoliageGroupData* FoliageGroupData, FFoliageGroupSettings* FoliageGroupSettings)
{
	InstanceFoliage(&FoliageGroupData->FoliageInstances,&FoliageGroupSettings->Foliage,&FoliageGroupData->FoliagePoints,FoliageGroupSettings->FoliageOffset,FoliageGroupSettings->FoliageScale,FoliageGroupSettings->ElevationMinMax,FoliageGroupSettings->SlopeMinMax,FoliageGroupSettings->EnableCollision);
	FoliageGroupData->hasFoliage = true;
}

//MISC

bool ATerrainChunk::EvaluateTerrain(FVector pos, FNoiseSettings* NoiseSettings, int heightMultiplier, FVector2D elevationLimits, FVector2D slopeLimits, FTerrainEvaluationData* data)
{
	float height = UNoise::Evaluate2D(pos,NoiseSettings) * heightMultiplier;
	FVector hpos = FVector(pos.X,pos.Y,height);
	float slope = EvaluateSlope(hpos,NoiseSettings,heightMultiplier);

	bool heightValid = false;
	bool slopeValid = false;

	if((elevationLimits.X <= height && elevationLimits.Y >= height) || elevationLimits.X == elevationLimits.Y) {
		heightValid = true;
	}
	if(slopeLimits.X <= slope && slopeLimits.Y >= slope)
	{
		slopeValid = true;
	}

	bool isValid = heightValid && slopeValid;

	if(data != nullptr)
	{
		data->valid = isValid;
		data->height = height;
		data->slope = slope;	
	}
	
	return isValid;	
}

float ATerrainChunk::EvaluateSlope(FVector pos,FNoiseSettings* NoiseSettings, int heightMultiplier)
{
	float height = pos.Z; 
	float heightX =  UNoise::Evaluate2D(pos+FVector(100,0,0),NoiseSettings) * heightMultiplier;
	float heightY =  UNoise::Evaluate2D(pos+FVector(0,100,0),NoiseSettings) * heightMultiplier;

	float slopeX = FMath::Abs(heightX - height) / 100;
	float slopeY = FMath::Abs(heightY - height) / 100;

	return slopeX > slopeY ? slopeX : slopeY;
}

bool ATerrainChunk::EvaluateTerrain(UTexture2D* Texture, FVector pos, int heightMultiplier, FVector2D elevationLimits, FVector2D slopeLimits, FTerrainEvaluationData* data)
{
	float height = UTextureUtils::SampleTextureR16f(Texture,FIntVector2(pos.X/100,pos.Y/100)) * heightMultiplier;
	FVector hpos = FVector(pos.X,pos.Y,height);
	float slope = EvaluateSlope(Texture,hpos,heightMultiplier);

	bool heightValid = false;
	bool slopeValid = false;

	if((elevationLimits.X <= height && elevationLimits.Y >= height) || elevationLimits.X == elevationLimits.Y) {
		heightValid = true;
	}
	if(slopeLimits.X <= slope && slopeLimits.Y >= slope) {
		slopeValid = true;
	}

	bool isValid = heightValid && slopeValid;

	if(data != nullptr)
	{
		
		data->valid = isValid;
		data->height = height;
		data->slope = slope;	
	}
	
	return isValid;	
}

float ATerrainChunk::EvaluateSlope(UTexture2D* Texture, FVector pos, int heightMultiplier)
{
	float height = pos.Z; 
	float heightX =  UTextureUtils::SampleTextureR16f(Texture,FIntVector2((pos.X+Settings.NoiseStepSize)/Settings.NoiseStepSize,pos.Y/Settings.NoiseStepSize)) * heightMultiplier;
	float heightY =  UTextureUtils::SampleTextureR16f(Texture,FIntVector2(pos.X/Settings.NoiseStepSize,(pos.Y+Settings.NoiseStepSize)/Settings.NoiseStepSize)) * heightMultiplier;

	float slopeX = FMath::Abs(heightX - height) / 100;
	float slopeY = FMath::Abs(heightY - height) / 100;

	return slopeX > slopeY ? slopeX : slopeY;
}

FVector ATerrainChunk::CoordToPos(FVector2D chunkCoord, int chunkSize)
{
	return FVector(chunkCoord.X * chunkSize, chunkCoord.Y * chunkSize,0);
}

FVector2D ATerrainChunk::PosToCoord(FVector pos, int chunkSize)
{
	return FVector2D(FMath::FloorToInt(pos.X / chunkSize), FMath::FloorToInt(pos.Y / chunkSize));
}

void ATerrainChunk::DebugDraw(float life, FColor color)
{
	if(!Settings.EnableDebugDraw) return;
	DebugDrawBounds(life,color);
	DebugDrawPositionMarker(life,color);
}

void ATerrainChunk::SetPosition(FVector2D chunkCoord, int chunkSize)
{
	Coord = chunkCoord;
	position = CoordToPos(chunkCoord,chunkSize);
	SetActorLocation(position);
}

void ATerrainChunk::DebugDrawBounds(float life, FColor color)
{
	FVector extends = FVector(Settings.Size.X/2,Settings.Size.Y/2,Settings.Size.Z/2);
	FVector pos = QTree.Center + FVector(0,0,-Settings.Size.Z/2);
	DrawDebugBox(GetWorld(),pos,extends,color,false,life,0u,0.0f);
}

void ATerrainChunk::DebugDrawPositionMarker(float life, FColor color)
{
	FVector extends = FVector(Settings.Size.X * 0.025f,Settings.Size.Y * 0.025f, -Settings.Size.Z/2);
	FVector displace = FVector(Settings.Size.X * 0.025f,Settings.Size.Y * 0.025f,0);
	FVector pos = position + displace;
	DrawDebugSolidBox(GetWorld(),pos,extends,color,false,life,0);
}