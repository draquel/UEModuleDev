#include "InfiniteTerrain.h"
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
		TerrainChunkMap.Add(key,CreateChunk(key))->Update(GetWorld()->IsGameWorld() ? PlayerPositionLastUpdate : GetPlayerPos(),false);
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

	FVector2D currentChunk = ATerrainChunk::PosToCoord(GetWorld()->IsGameWorld() ? PlayerPositionLastUpdate : GetPlayerPos(),TerrainChunkSettings.Size.X);
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
		if(chunk.Value->IsHidden())	{
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
	
	if (!GetWorld()->IsGameWorld())	{
		while (!AddKeys.IsEmpty() || !RemoveKeys.IsEmpty() || !UpdateKeys.IsEmpty() || !DisableKeys.IsEmpty()) {
			ProcessChunkQueues(false);
		}
	}
	
	double end = FPlatformTime::Seconds();

	UE_LOG(TerrainGenerator,Log,TEXT("InfiniteTerrain::UpdateChunks() ==> Queues(toAdd: %d toUpdate: %d toDisable: %d toRemove: %d), Chunks(Loaded: %d Hidden: %d Total: %d) RunTime: %fs"),newCount,updateCount,disableCount,removeCount,TerrainChunkMap.Num()-(disabledKeys.Num()+removeCount),disabledKeys.Num()+removeCount,TerrainChunkMap.Num(),end-start);
}

void AInfiniteTerrain::UpdateFoliage()
{
	for(auto Chunk : TerrainChunkMap)
	{
		Chunk.Value->Settings.FoliageGroups = TerrainChunkSettings.FoliageGroups;
		Chunk.Value->CreateFoliage(GetPlayerPos(),false, true);
	} 	
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

	ProcessChunkQueues(true);
}

void AInfiniteTerrain::ProcessChunkQueues(bool allowThreads = false)
{
	int updateCount = 0;

	if(!UpdateKeys.IsEmpty() && !isUpdatingChunks) {
		int i;
		for(i = updateCount; i < UpdateResolution; i++) {
			if(!UpdateKeys.Peek()){ break; }
			FVector2D key;
			UpdateKeys.Dequeue(key);
			ATerrainChunk* chunk = *TerrainChunkMap.Find(key);
			if(chunk->IsHidden()){ chunk->Enable(); }
			else { chunk->Update(GetWorld()->IsGameWorld() ? PlayerPositionLastUpdate : GetPlayerPos(), allowThreads); }
			UE_LOG(TerrainGenerator,VeryVerbose,TEXT("InfiniteTerrain::ProcessChunkQueues() => Chunk %s Updated"),*key.ToString());
		}
		UE_LOG(TerrainGenerator,Log,TEXT("InfiniteTerrain::ProcessChunkQueues() Thread => Updating %i Chunks"),i);
	}
	
	if(!AddKeys.IsEmpty() && !isUpdatingChunks) {
		int i;
		for(i = updateCount; i < UpdateResolution; i++) {
			if(!AddKeys.Peek()){ break; }
			FVector2D key;
			AddKeys.Dequeue(key);		
			TerrainChunkMap.Add(key,CreateChunk(key));
			UpdateKeys.Enqueue(key);
			UE_LOG(TerrainGenerator,VeryVerbose,TEXT("InfiniteTerrain::ProcessChunkQueues() => Chunk %s Added"),*key.ToString());
		}
		UE_LOG(TerrainGenerator,Log,TEXT("InfiniteTerrain::ProcessChunkQueues() Thread => Adding %i Chunks"),i);
	}
	
	if(!DisableKeys.IsEmpty() && !isUpdatingChunks) {
		int i;
		for(i = updateCount; i < UpdateResolution; i++) {
			if(!DisableKeys.Peek()){ break; }
			FVector2D key;
			DisableKeys.Dequeue(key);
			ATerrainChunk* chunk = *TerrainChunkMap.Find(key);
			chunk->Disable();
			UE_LOG(TerrainGenerator,VeryVerbose,TEXT("InfiniteTerrain::ProcessChunkQueues() => Chunk %s Disabled"),*key.ToString());
		}
		UE_LOG(TerrainGenerator,Log,TEXT("InfiniteTerrain::ProcessChunkQueues() Thread => Disabling %i Chunks"),i);
	}

	if(!RemoveKeys.IsEmpty() && !isUpdatingChunks) {
		int i;
		for(i = updateCount; i < UpdateResolution; i++) {
			if(!RemoveKeys.Peek()){ break; }
			FVector2D key;
			RemoveKeys.Dequeue(key);
			ATerrainChunk* c = *TerrainChunkMap.Find(key);
			c->Destroy();
			TerrainChunkMap.Remove(key);
			UE_LOG(TerrainGenerator,VeryVerbose,TEXT("InfiniteTerrain::ProcessChunkQueues() => Chunk %s Destroyed"),*key.ToString())
		}
		UE_LOG(TerrainGenerator,Log,TEXT("InfiniteTerrain::ProcessChunkQueues() Thread => Destroying %i Chunks"),i);
	}
}

FVector AInfiniteTerrain::GetPlayerPos() const
{
	if(GetWorld()->IsGameWorld()) {
		return GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	}
#if WITH_EDITOR
	if (GEditor && GEditor->GetActiveViewport()) {
		FViewport* Viewport = GEditor->GetActiveViewport();
		FEditorViewportClient* ViewportClient = (FEditorViewportClient*)Viewport->GetClient();

		if (ViewportClient) {
			return ViewportClient->GetViewLocation();
			// FRotator CameraRotation = ViewportClient->GetViewRotation();
			// UE_LOG(LogTemp, Log, TEXT("Editor Camera Location: %s"), *CameraLocation.ToString());
			// UE_LOG(LogTemp, Log, TEXT("Editor Camera Rotation: %s"), *CameraRotation.ToString());
		}
	}
#endif
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

