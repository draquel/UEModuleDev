#pragma once

#include "CoreMinimal.h"
#include "TerrainChunk.h"
#include "TerrainChunkSettings.h"
#include "GameFramework/Actor.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "InfiniteTerrain.generated.h"

class ATerrainChunk;

UCLASS()
class TERRAINGENERATOR_API AInfiniteTerrain : public AActor
{
	GENERATED_BODY()
	
public:	
	AInfiniteTerrain();

	UPROPERTY(EditAnywhere)
	int ChunkRenderDistance = 1;

	UPROPERTY(EditAnywhere)
	FTerrainChunkSettings TerrainChunkSettings;

	UPROPERTY(EditAnywhere)
	FVector PlayerPositionLastUpdate;
	
	UPROPERTY(EditAnywhere)
	int32 UpdateResolution = 1;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	bool enableDayNightCycle = false;

	UPROPERTY(VisibleAnywhere)
	bool isTerrainReady = false;
	
	TQueue<FVector2D> AddKeys;
	TQueue<FVector2D> UpdateKeys;
	TQueue<FVector2D> DisableKeys; 
	TQueue<FVector2D> RemoveKeys;
	
protected:
	UPROPERTY(VisibleAnywhere)
	TMap<FVector2D,ATerrainChunk*> TerrainChunkMap;

	bool isUpdatingChunks = false;
	
	virtual void BeginPlay() override;
	
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(VisibleAnywhere)
	ANavMeshBoundsVolume* NavMeshVolume;

	ATerrainChunk* CreateChunk(FVector2D chunkCoord);
	ANavMeshBoundsVolume* CreateNavMesh();
	void GetNavMeshVolume();

	UFUNCTION(CallInEditor, meta=(Category="InfiniteTerrain"))
	void NavMeshUpdate();

	void Generate();
	
	UFUNCTION(CallInEditor, meta=(Category="InfiniteTerrain"))
	void Regenerate();

	UFUNCTION(CallInEditor, meta=(Category="InfiniteTerrain"))
	void UpdateChunks();

	UFUNCTION(CallInEditor, meta=(Category="InfiniteTerrain"))
	void ClearChunks();
	
	void ClearQueues();
	
	bool PlayerUpdateDistanceCheck() const;

	UFUNCTION(CallInEditor, meta=(Category="InfiniteTerrain"))
	void ShowChunkBorders();

public:	
	virtual void Tick(float DeltaTime) override;
	FVector GetPlayerPos() const;
};
