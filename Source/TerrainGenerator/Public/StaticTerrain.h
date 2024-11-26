#pragma once

#include "CoreMinimal.h"
#include "TerrainChunk.h"
#include "TerrainChunkSettings.h"
#include "GameFramework/Actor.h"
#include "StaticTerrain.generated.h"

UCLASS()
class TERRAINGENERATOR_API AStaticTerrain : public AActor
{
	GENERATED_BODY()
	
public:	
	AStaticTerrain();

	UPROPERTY(EditAnywhere)
	int32 width;

	UPROPERTY(EditAnywhere)
	FTerrainChunkSettings TerrainChunkSettings;

	UPROPERTY(EditAnywhere)
	FVector PlayerPositionLastUpdate;

	UPROPERTY(EditAnywhere)
	bool AutoUpdate;

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(CallInEditor, meta=(Category="Static Terrain"))
	void ClearChunks();

	void Generate();

	UFUNCTION(CallInEditor, meta=(Category="Static Terrain"))
	void Regenerate();

	ATerrainChunk* CreateChunk(FVector2D chunkCoord);
	
	TMap<FVector2D,ATerrainChunk*> TerrainChunkMap;

public:	
	virtual void Tick(float DeltaTime) override;

};
