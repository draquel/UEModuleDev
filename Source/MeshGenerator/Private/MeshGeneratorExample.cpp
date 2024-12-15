#include "MeshGeneratorExample.h"

#include "LevelEditorViewport.h"
#include "MeshGenerator.h"
#include "Noise.h"
#include "Engine/TextureRenderTarget2D.h"

#include "MeshData/MeshDataGenerator.h"
#include "NoiseTextureComputeShader/NoiseTextureComputeShader.h"


// Sets default values
AMeshGeneratorExample::AMeshGeneratorExample()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	
	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>("Mesh");
	Mesh->SetCanEverAffectNavigation(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AMeshGeneratorExample::BeginPlay()
{
	Super::BeginPlay();
	Generate();
}

void AMeshGeneratorExample::Generate()
{
	double start = FPlatformTime::Seconds();
	Mesh->SetMaterial(0,Material);
	UTextureRenderTarget2D* RT = CreateRenderTarget(RTF_R16f);
	switch (Mode) {
		default:
		case 0:
			if (DataMode2D == RenderTexture) {
				bool GenerateNoiseMap = false;
				if (NoiseTexture == nullptr || !FNoiseSettings::Compare(LastNoiseSettings, NoiseSettings2D)) { GenerateNoiseMap = true;	}
				if (GenerateNoiseMap)
				{
					UNoise::GenerateTexture(RT,(FIntVector)GetActorLocation(),FIntVector2(Size.X,Size.Y),StepSize,&NoiseSettings2D,[this,RT,start]() {
						NoiseTexture = RT->ConstructTexture2D(GetWorld(), "SavedDataTexture", RF_Public | RF_Standalone);
						FMeshData Data = UMeshDataGenerator::RectMesh(NoiseTexture,GetActorLocation(),Size,RectMesh_StepSize,StepSize,UVScale,FMath::FloorToInt(Size.Z));
						UpdateMesh(&Data);
						double end = FPlatformTime::Seconds();
						UE_LOG(MeshGenerator,Log,TEXT("AMeshGeneratorExample::Generate ==> Rect_2D RenderTexture[Gen] ==> Size:%s, RunTime: %fs"),*(Size/StepSize).ToString(),end-start);
					});
				}else {
					FMeshData Data = UMeshDataGenerator::RectMesh(NoiseTexture,GetActorLocation(),Size,RectMesh_StepSize,StepSize,UVScale, FMath::FloorToInt(Size.Z));
					UpdateMesh(&Data);		
					double end = FPlatformTime::Seconds();
					UE_LOG(MeshGenerator,Log,TEXT("AMeshGeneratorExample::Generate ==> Rect_2D RenderTexture[Regen] ==> Size:%s, RunTime: %fs"),*(Size/StepSize).ToString(),end-start);
				}
			}
			if (DataMode2D == NoiseMap)	{
				UNoise::GenerateMap2D((FIntVector)GetActorLocation(),FIntVector2(Size.X+StepSize,Size.Y+StepSize)/StepSize,StepSize,NoiseSettings2D,[this,start](FNoiseMap2d NoiseMap) {
					this->NoiseMap2D = &NoiseMap;
					FMeshData Data = UMeshDataGenerator::RectMesh(NoiseMap2D,GetActorLocation(),Size,UVScale,FMath::FloorToInt(Size.Z));
					UpdateMesh(&Data);	
					double end = FPlatformTime::Seconds();
					UE_LOG(MeshGenerator,Log,TEXT("AMeshGeneratorExample::Generate ==> Rect_2D NoiseMap ==> Size:%s, RunTime: %fs"),*(Size/StepSize).ToString(),end-start);
				});
			}
			LastNoiseSettings = NoiseSettings2D;
			break;
		case 1:
			if (DataMode2D == RenderTexture) {
				bool GenerateNoiseMap = false;
				if (NoiseTexture == nullptr || !FNoiseSettings::Compare(LastNoiseSettings, NoiseSettings2D)) { GenerateNoiseMap = true;	}
				if (GenerateNoiseMap) {
					UNoise::GenerateTexture(RT,(FIntVector)GetActorLocation(),FIntVector2(Size.X,Size.Y),StepSize,&NoiseSettings2D,[this,RT,start]() {
						UQuadTree QuadTree = UQuadTree(GetActorLocation(),Size,QuadTreeSettings);
						QuadTree.GenerateTree(GetActorLocation());
					
						NoiseTexture = RT->ConstructTexture2D(GetWorld(), "SavedDataTexture", RF_Public | RF_Standalone);
						FMeshData Data = UMeshDataGenerator::QuadTreeMesh(NoiseTexture,&QuadTree,StepSize,UVScale, 0,FMath::FloorToInt(Size.Z));
						UpdateMesh(&Data);
						double end = FPlatformTime::Seconds();
						UE_LOG(MeshGenerator,Log,TEXT("AMeshGeneratorExample::Generate ==> QuadTree_2D RenderTexture[Gen] ==> Size:%s, RunTime: %fs"),*(Size/StepSize).ToString(),end-start);
					});	
				}else{
					UQuadTree QuadTree = UQuadTree(GetActorLocation(),Size,QuadTreeSettings);
					QuadTree.GenerateTree(GetActorLocation());
					FMeshData Data = UMeshDataGenerator::QuadTreeMesh(NoiseTexture,&QuadTree,StepSize,UVScale, 0,FMath::FloorToInt(Size.Z));
					UpdateMesh(&Data);
					double end = FPlatformTime::Seconds();
					UE_LOG(MeshGenerator,Log,TEXT("AMeshGeneratorExample::Generate ==> QuadTree_2D RenderTexture[Regen] ==> Size:%s, RunTime: %fs"),*(Size/StepSize).ToString(),end-start);
				}
			}
			
			if (DataMode2D == NoiseMap)
			{
				UNoise::GenerateMap2D((FIntVector)GetActorLocation(),FIntVector2(Size.X+StepSize,Size.Y+StepSize),StepSize,NoiseSettings2D,[this,start](FNoiseMap2d NoiseMap)	{
					this->NoiseMap2D = &NoiseMap; 
					UQuadTree QuadTree = UQuadTree(GetActorLocation(),Size,QuadTreeSettings);
					QuadTree.GenerateTree(GetActorLocation());
					FMeshData Data = UMeshDataGenerator::QuadTreeMesh(NoiseMap2D,&QuadTree,UVScale,0,(int)FMath::FloorToInt(Size.Z));
					UpdateMesh(&Data);	
					double end = FPlatformTime::Seconds();
					UE_LOG(MeshGenerator,Log,TEXT("AMeshGeneratorExample::Generate ==> QuadTree_2D NoiseMap ==> Size:%s, RunTime: %fs"),*(Size/StepSize).ToString(),end-start);
				});	
			}
			LastNoiseSettings = NoiseSettings2D;
			break;
		case 2:
			UNoise::GenerateMap3D((FIntVector)GetActorLocation(),(FIntVector)(Size+StepSize),StepSize,NoiseSettings3D,Floor,[this](FNoiseMap3d NoiseMap){
				this->NoiseMap3D = &NoiseMap; 
				FMeshData Data = UMeshDataGenerator::MarchingCubes(NoiseMap3D,GetActorLocation(),Size,StepSize,UVScale,MarchingCubesSettings);
				UpdateMesh(&Data);	
			});
			LastNoiseSettings = NoiseSettings3D;
			break;
	}
}

void AMeshGeneratorExample::Regenerate()
{
	FMeshData Data;
	UQuadTree QuadTree = UQuadTree(GetActorLocation(),Size,QuadTreeSettings);
	switch (Mode) {
		default:
		case 0:
			Data = UMeshDataGenerator::RectMesh(NoiseMap2D,GetActorLocation(),Size,UVScale,FMath::FloorToInt(Size.Z));
			break;
		case 1:
			QuadTree.GenerateTree(GetPlayerPos());
			Data = UMeshDataGenerator::QuadTreeMesh(NoiseMap2D,&QuadTree,UVScale,0,FMath::FloorToInt(Size.Z));
			break;
		case 2:
			Data = UMeshDataGenerator::MarchingCubes(NoiseMap3D,GetActorLocation(),Size,StepSize,UVScale,MarchingCubesSettings);
			break;
	}	
	UpdateMesh(&Data);		
}

void AMeshGeneratorExample::UpdateMesh(FMeshData* Data)
{		
	double start = FPlatformTime::Seconds();
	
	Data->CreateProceduralMesh(Mesh);
	SetActorEnableCollision(true);
	Data->ConfigureForNavigation(Mesh);
	
	DebugDraw();
	
	int verts = Data->Vertices.Num(); 
	Data->Reset();
	
	double end = FPlatformTime::Seconds();
	UE_LOG(MeshGenerator,Log,TEXT("AMeshGeneratorExample::UpdateMesh ==> Verts:%i, RunTime: %fs"),verts,end-start);
}

// Called every frame
void AMeshGeneratorExample::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMeshGeneratorExample::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!bHasBeenConstructed){
		bHasBeenConstructed = true;
		UE_LOG(MeshGenerator, Display, TEXT("MeshGeneratorExample::OnConstruction() - Initialized"));
		return;
	}
	if (autoUpdate)	Generate();
}

void AMeshGeneratorExample::DebugDraw()
{
	FlushPersistentDebugLines(GetWorld());
	float life = 0.0f;
	FColor color = FColor::Red;
	DebugDrawBounds(life,color);
	DebugDrawPositionMarker(life,color);
}
void AMeshGeneratorExample::DebugDrawBounds(float life, FColor color)
{
	FVector extends = FVector(Size.X/2,Size.Y/2,Size.Z/2);
	FVector pos = GetActorLocation() + FVector(Size.X/2,Size.Y/2,Size.Z/2);
	DrawDebugBox(GetWorld(),pos,extends,color,true,life,0u,0.0f);
}

void AMeshGeneratorExample::DebugDrawPositionMarker(float life, FColor color)
{
	FVector extends = FVector(Size.X * 0.0125f,Size.Y * 0.0125f, Size.Z/2);
	FVector displace = FVector(Size.X * 0.0125f,Size.Y * 0.0125f,Size.Z/2);
	FVector pos = GetActorLocation()  + displace;
	DrawDebugSolidBox(GetWorld(),pos,extends,color,true,life,0);
}

void AMeshGeneratorExample::Clear()
{
	Mesh->ClearMeshSection(0);
	FlushPersistentDebugLines(GetWorld());
	NoiseTexture = nullptr;
}

FVector AMeshGeneratorExample::GetPlayerPos() const
{
	if(GetWorld()->IsGameWorld()) {
		return GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	}
	if(GCurrentLevelEditingViewportClient) {
		FViewportCameraTransform ViewTransform = GCurrentLevelEditingViewportClient->GetViewTransform();
		return ViewTransform.GetLocation();
	}
	return FVector();
}

UTextureRenderTarget2D* AMeshGeneratorExample::CreateRenderTarget(ETextureRenderTargetFormat Format)
{
	// Create the render target
	UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>();
    
	// Set the render target properties
	RenderTarget->RenderTargetFormat = Format;
	RenderTarget->InitAutoFormat((Size.X+StepSize)/StepSize, (Size.Y+StepSize)/StepSize);
	RenderTarget->ClearColor = FLinearColor::Black;
	RenderTarget->bAutoGenerateMips = false;
	RenderTarget->UpdateResourceImmediate(true);

	return RenderTarget;
}