#include "InfiniteTerrain.h"
#include "LevelEditorViewport.h"
#include "NavigationSystem.h"
#include "Builders/CubeBuilder.h"
#include "Components/BrushComponent.h"
#include "Engine/BrushBuilder.h"
#include "Kismet/GameplayStatics.h"
#include "TerrainChunk.h"
#include "TerrainGenerator.h"
#include "NavMesh/NavMeshBoundsVolume.h"

class FEditorViewportClient;

AInfiniteTerrain::AInfiniteTerrain()
{
	PrimaryActorTick.bCanEverTick = true;
	PlayerPositionLastUpdate = FVector();
	SetActorTickInterval(0.5f);
}

void AInfiniteTerrain::BeginPlay()
{
	Super::BeginPlay();
	isTerrainReady = false;
	PlayerPositionLastUpdate = GetPlayerPos();
	Regenerate();
}


void AInfiniteTerrain::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

ATerrainChunk* AInfiniteTerrain::CreateChunk(FVector2D chunkCoord)
{
	ATerrainChunk* DeferedChunk = GetWorld()->SpawnActorDeferred<ATerrainChunk>(ATerrainChunk::StaticClass(),FTransform());
	DeferedChunk->Init(chunkCoord,TerrainChunkSettings);
	ATerrainChunk* SpawnedChunk = (ATerrainChunk*)UGameplayStatics::FinishSpawningActor(DeferedChunk,DeferedChunk->GetTransform());
	SpawnedChunk->SetFolderPath("/Terrain");
	SpawnedChunk->SetPosition(chunkCoord,TerrainChunkSettings.Size.X);
	SpawnedChunk->DebugDraw(0.5f,FColor::Green);
	return SpawnedChunk;
}

ANavMeshBoundsVolume* AInfiniteTerrain::CreateNavMesh()
{
	FTransform nmvTransform = FTransform();
	nmvTransform.SetLocation(GetPlayerPos());
	nmvTransform.SetScale3D(FVector::One());
	
	ANavMeshBoundsVolume* defferedNmv = GetWorld()->SpawnActorDeferred<ANavMeshBoundsVolume>(ANavMeshBoundsVolume::StaticClass(),nmvTransform);
	defferedNmv->GetBrushComponent()->SetMobility(EComponentMobility::Type::Movable);
	UCubeBuilder* CubeBuilder = Cast<UCubeBuilder>(GEditor->FindBrushBuilder(UCubeBuilder::StaticClass()));
	CubeBuilder->X = TerrainChunkSettings.Size.X; 
	CubeBuilder->Y = TerrainChunkSettings.Size.Y;
	CubeBuilder->Z = TerrainChunkSettings.Size.Z;
	defferedNmv->BrushBuilder = CubeBuilder;
	defferedNmv->BrushBuilder->Build(GetWorld());
	
	if(GetWorld()->IsGameWorld()) {
		defferedNmv->AttachToActor(GetWorld()->GetFirstPlayerController()->GetPawn(),FAttachmentTransformRules(EAttachmentRule::SnapToTarget,EAttachmentRule::KeepRelative,EAttachmentRule::KeepRelative,true));
	}
	NavMeshVolume = (ANavMeshBoundsVolume*)UGameplayStatics::FinishSpawningActor(defferedNmv,defferedNmv->GetTransform());

	NavMeshUpdate();

	return NavMeshVolume;
}

void AInfiniteTerrain::GetNavMeshVolume()
{
	TArray<AActor*> NavMeshArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),ANavMeshBoundsVolume::StaticClass(),NavMeshArray);
	if(NavMeshArray.Num() > 0)	{
		NavMeshVolume = (ANavMeshBoundsVolume*)NavMeshArray[0];
	}
	if(GetWorld()->IsGameWorld()) {
		NavMeshVolume->AttachToActor(GetWorld()->GetFirstPlayerController()->GetPawn(),FAttachmentTransformRules(EAttachmentRule::SnapToTarget,EAttachmentRule::KeepRelative,EAttachmentRule::KeepRelative,true));
	}
}

void AInfiniteTerrain::NavMeshUpdate()
{
	if(NavMeshVolume == nullptr){ GetNavMeshVolume(); }
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (NavSys) {
		if(NavSys->IsNavigationBuildingLocked()) {
			NavSys->ReleaseInitialBuildingLock();
		}
		if(NavSys->IsNavigationDirty())	{
			NavSys->Build();
		}
		NavSys->OnNavigationBoundsUpdated(NavMeshVolume);
	}
}

void AInfiniteTerrain::Generate()
{
	double start = FPlatformTime::Seconds();
	isUpdatingChunks = true;
	
	FVector2D currentChunk = ATerrainChunk::PosToCoord(GetPlayerPos(),TerrainChunkSettings.Size.X);

	//Add Initial Chunks
	int x = 0;
	int y = 0;
	for(int i = 0; i < (ChunkRenderDistance == -1 ? 0 : (ChunkRenderDistance*2+1)*(ChunkRenderDistance*2+1)); ++i) {
		FVector2D chunkCoord = currentChunk + FVector2D(x,y);
		if(FVector2D::Distance(currentChunk,chunkCoord) <= ChunkRenderDistance+1 && !TerrainChunkMap.Contains(chunkCoord))	{
			AddKeys.Enqueue(chunkCoord);
		}
		
		if(abs(x) <= abs(y) && (x != y || x >= 0)){	x += ((y >= 0) ? 1 : -1); }
		else { y += ((x >= 0) ? -1 : 1); }
	}

	FVector2D key;
	while (AddKeys.Peek() != nullptr) {
		AddKeys.Dequeue(key);
		TerrainChunkMap.Add(key,CreateChunk(key))->Update(PlayerPositionLastUpdate,false);
	}

	NavMeshUpdate();	
	
	isUpdatingChunks = false;
	isTerrainReady = true;
	double end = FPlatformTime::Seconds();
	
	UE_LOG(TerrainGenerator,Log,TEXT("InfiniteTerrain::Generate() ==> Chunks(Center: %s Total: %d) RunTime: %fs"),*currentChunk.ToString(),TerrainChunkMap.Num(),end-start);	
}

void AInfiniteTerrain::UpdateChunks()
{
	double start = FPlatformTime::Seconds();
	isUpdatingChunks = true;

	FVector2D currentChunk = ATerrainChunk::PosToCoord(PlayerPositionLastUpdate,TerrainChunkSettings.Size.X);
	int limit = ChunkRenderDistance * ChunkRenderDistance;

	ClearQueues();	
	
	int newCount = 0;
	int updateCount = 0;
	int disableCount = 0;
	int removeCount = 0;
	
	//Add/Update Visible Chunks
	int x = 0;
	int y = 0;
	for(int i = 0; i < (ChunkRenderDistance*2+1)*(ChunkRenderDistance*2+1); ++i) {
		FVector2D chunkCoord = currentChunk + FVector2D(x,y);
		if(FVector2D::Distance(currentChunk,chunkCoord) <= ChunkRenderDistance+1){
			if(!TerrainChunkMap.Contains(chunkCoord)) {
				AddKeys.Enqueue(chunkCoord);
				newCount++;
			}else {
				UpdateKeys.Enqueue(chunkCoord);
				updateCount++;
			}
		}
		
		if(abs(x) <= abs(y) && (x != y || x >= 0)){	x += ((y >= 0) ? 1 : -1); }
		else { y += ((x >= 0) ? -1 : 1); }
	}

	//Disable/Remove Chunks
	TArray<FVector2D> disabledKeys;
	for (auto chunk : TerrainChunkMap) {
		if(FVector2D::Distance(currentChunk,chunk.Key) <= ChunkRenderDistance+1) { continue; }
		if(!chunk.Value->IsHidden())	{
			disabledKeys.Add(chunk.Key);
		} else {
			DisableKeys.Enqueue(chunk.Key);
			disableCount++;
		}
	}
	if(disabledKeys.Num() > limit) {
		disabledKeys.Sort([currentChunk](const FVector2D A, const FVector2D B){ return FVector2D::Distance(A,currentChunk) < FVector2D::Distance(B,currentChunk); });
		while(disabledKeys.Num() > limit) {
			RemoveKeys.Enqueue(disabledKeys.Pop());
			removeCount++;
		}
	}
	
	isUpdatingChunks = false;
	double end = FPlatformTime::Seconds();

	UE_LOG(TerrainGenerator,Log,TEXT("InfiniteTerrain::UpdateChunks() ==> Queues(toAdd: %d toUpdate: %d toDisable: %d toRemove: %d), Chunks(Loaded: %d Hidden: %d Total: %d) RunTime: %fs"),newCount,updateCount,disableCount,removeCount,TerrainChunkMap.Num()-(disabledKeys.Num()+removeCount),disabledKeys.Num()+removeCount,TerrainChunkMap.Num(),end-start);
}

void AInfiniteTerrain::ClearChunks()
{
	for(auto Chunk : TerrainChunkMap){ Chunk.Value->Destroy(); } 
	TerrainChunkMap.Reset();
	ClearQueues();
}

void AInfiniteTerrain::ClearQueues()
{
	AddKeys.Empty();
	UpdateKeys.Empty();
	DisableKeys.Empty();
	RemoveKeys.Empty();	
}

void AInfiniteTerrain::Regenerate()
{
	ClearChunks();
	Generate();
}

void AInfiniteTerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(PlayerUpdateDistanceCheck() && !isUpdatingChunks) {
		NavMeshUpdate();
		PlayerPositionLastUpdate = GetPlayerPos();
		UpdateChunks();
	}

	int updateCount = 0;	
	if(!AddKeys.IsEmpty() && !isUpdatingChunks) {
		double start = FPlatformTime::Seconds();
		int i;
		for(i = updateCount; i < UpdateResolution; i++) {
			if(!AddKeys.Peek()){ break; }
			FVector2D key;
			AddKeys.Dequeue(key);		
			TerrainChunkMap.Add(key,CreateChunk(key));
			UE_LOG(TerrainGenerator,VeryVerbose,TEXT("InfiniteTerrain::UpdateChunks() => Chunk %s Added"),*key.ToString());
		}
		double end = FPlatformTime::Seconds();
		UE_LOG(TerrainGenerator,Log,TEXT("InfiniteTerrain::UpdateChunks() Thread => Added %i Chunks in %f"),i,end-start);
	}
	
	if(!UpdateKeys.IsEmpty() && !isUpdatingChunks) {
		double start = FPlatformTime::Seconds();
		int i;
		for(i = updateCount; i < UpdateResolution; i++) {
			if(!UpdateKeys.Peek()){ break; }
			FVector2D key;
			UpdateKeys.Dequeue(key);
			ATerrainChunk* chunk = *TerrainChunkMap.Find(key);
			if(chunk->IsHidden()){ chunk->Enable(); }
			else { chunk->Update(PlayerPositionLastUpdate); }
			UE_LOG(TerrainGenerator,VeryVerbose,TEXT("InfiniteTerrain::UpdateChunks() => Chunk %s Updated"),*key.ToString());
		}
		double end = FPlatformTime::Seconds();
		UE_LOG(TerrainGenerator,Log,TEXT("InfiniteTerrain::UpdateChunks() Thread => Updated %i Chunks in %f"),i,end-start);
	}

	if(!DisableKeys.IsEmpty() && !isUpdatingChunks) {
		for(int i = updateCount; i < UpdateResolution; i++) {
			if(!DisableKeys.Peek()){ break; }
			FVector2D key;
			DisableKeys.Dequeue(key);
			ATerrainChunk* chunk = *TerrainChunkMap.Find(key);
			chunk->Disable();
			UE_LOG(TerrainGenerator,VeryVerbose,TEXT("InfiniteTerrain::UpdateChunks() => Chunk %s Disabled"),*key.ToString());
		}
	}

	if(!RemoveKeys.IsEmpty() && !isUpdatingChunks) {
		double start = FPlatformTime::Seconds();
		int i;
		for(i = updateCount; i < UpdateResolution; i++) {
			if(!RemoveKeys.Peek()){ break; }
			FVector2D key;
			RemoveKeys.Dequeue(key);
			ATerrainChunk* c = *TerrainChunkMap.Find(key);
			c->DebugDraw(0.5f,FColor::Red);
			c->Destroy();
			TerrainChunkMap.Remove(key);
			UE_LOG(TerrainGenerator,VeryVerbose,TEXT("InfiniteTerrain::UpdateChunks() => Chunk %s Destroyed"),*key.ToString());
		}
		double end = FPlatformTime::Seconds();
		UE_LOG(TerrainGenerator,Log,TEXT("InfiniteTerrain::UpdateChunks() Thread => Destroyed %i Chunks in %f"),i,end-start);
	}
}

FVector AInfiniteTerrain::GetPlayerPos() const
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

bool AInfiniteTerrain::PlayerUpdateDistanceCheck() const
{
	return FVector::Distance(PlayerPositionLastUpdate, GetPlayerPos()) >
		TerrainChunkSettings.QuadTreeSettings.MinSize * (TerrainChunkSettings.QuadTreeSettings.DistanceModifier == 1 ? 1 : TerrainChunkSettings.QuadTreeSettings.DistanceModifier-1) * 1.5;
}

void AInfiniteTerrain::ShowChunkBorders()
{
	for (auto Element : TerrainChunkMap) {
		Element.Value->DebugDraw();
	}
}

