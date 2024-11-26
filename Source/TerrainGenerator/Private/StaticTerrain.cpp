#include "StaticTerrain.h"
#include "Kismet/GameplayStatics.h"
#include "TerrainChunk.h"

AStaticTerrain::AStaticTerrain()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoUpdate = false;
}

void AStaticTerrain::BeginPlay()
{
	Super::BeginPlay();
	ClearChunks();
	Generate();
}

void AStaticTerrain::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (AutoUpdate)	{
		ClearChunks();
		Generate();
	}
}

void AStaticTerrain::ClearChunks()
{
	for(auto Chunk : TerrainChunkMap){ Chunk.Value->Destroy(); } 
	TerrainChunkMap.Reset();
}

void AStaticTerrain::Generate()
{
	if(width == 0){ return; }
	for(int x = -width; x <= width; x++) {
		for(int y = -width; y <= width; y++) {
			FVector2D chunkCoord = FVector2D(x,y);
			TerrainChunkMap.Add(chunkCoord,CreateChunk(chunkCoord))->Update();
		}
	}
}

void AStaticTerrain::Regenerate()
{
	ClearChunks();
	Generate();
}

ATerrainChunk* AStaticTerrain::CreateChunk(FVector2D chunkCoord)
{
	ATerrainChunk* DeferedChunk = GetWorld()->SpawnActorDeferred<ATerrainChunk>(ATerrainChunk::StaticClass(),FTransform());
	ATerrainChunk* SpawnedChunk = (ATerrainChunk*)UGameplayStatics::FinishSpawningActor(DeferedChunk,DeferedChunk->GetTransform());
	DeferedChunk->Init(chunkCoord,TerrainChunkSettings);
	SpawnedChunk->SetFolderPath("/Terrain");
	return SpawnedChunk;
}
void AStaticTerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

