#pragma once

#include "CoreMinimal.h"
#include "TerrainChunkSettings.h"
#include "Components/DynamicMeshComponent.h"
#include "QuadTree/QuadTree.h"
#include "MeshData/MeshData.h"
#include "GameFramework/Actor.h"
#include "MeshData/Threads/FQuadTreeMeshDataThread.h"
#include "MeshData/Threads/FRectMeshDataThread.h"
#include "TerrainChunk.generated.h"

class FPoissonGeneratorThread;

USTRUCT()
struct FTerrainEvaluationData
{
	GENERATED_BODY();
	
	bool valid;
	float height;
	float slope;
};

UCLASS()
class TERRAINGENERATOR_API ATerrainChunk : public AActor
{
	GENERATED_BODY()
	
public:	
	ATerrainChunk();
	static bool EvaluateTerrain(FVector pos, FNoiseSettings* NoiseSettings, int heightMultiplier, FVector2D elevationLimits, FVector2D slopeLimits, FTerrainEvaluationData* data);
	
	UPROPERTY(EditAnywhere)
	FVector2D Coord;
	
	UPROPERTY(VisibleAnywhere)
	FVector position;

	UPROPERTY(EditAnywhere)
	FTerrainChunkSettings Settings;

	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* WaterMesh;

	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* TerrainMesh;

	UPROPERTY(VisibleAnywhere)
	UDynamicMeshComponent* DynamicMesh;
	
	UPROPERTY(EditAnywhere)
	TArray<FFoliageGroupData> FoliageGroups;

	UPROPERTY(VisibleAnywhere)
	UDecalComponent* WaterCausticsDecal;

	bool hasWater = false;

	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	void Init();
	void Init(FVector2D chunkCoord, FTerrainChunkSettings settings);
	void SetPosition(FVector2D chunkCoord,int chunkSize);	

	UFUNCTION(CallInEditor)
	void Update(FVector playerPos = FVector(),bool allowThreads = true);
	void Disable();
	void Enable();
	
	void CreateTerrainMesh(FVector playerPos, bool allowThread = true);
	void CreateQuadtreeMesh(FVector playerPos, bool allowThread = true);
	void CreateRectMesh(bool allowThread = true);
	void CreateWaterMesh(bool allowThread = true);
	void CreateFoliage(FVector playerPos);
	void InstanceFoliage(TArray<UInstancedStaticMeshComponent*>* StaticMeshComponents, TArray<UStaticMesh*>* Foliage, TArray<FVector2D>* Points, FVector Offset, FVector Scale, bool Collision);
	void InstanceFoliage(FFoliageGroupData* FoliageGroupData, FFoliageGroupSettings* FoliageGroupSettings);

	UFUNCTION(BlueprintCallable)
	void DebugDraw(float life = 30, FColor color = FColor::Red);
	UFUNCTION(BlueprintCallable)
	static FVector CoordToPos(FVector2D chunkCoord, int chunkSize);
	UFUNCTION(BlueprintCallable)
	static FVector2D PosToCoord(FVector pos, int chunkSize);

protected:
	virtual void BeginPlay() override;
	
	virtual void OnConstruction(const FTransform& Transform) override;
	
	QuadTree QTree;

	void DebugDrawBounds(float life = 30, FColor color = FColor::Red);
	void DebugDrawPositionMarker(float life = 30, FColor color = FColor::Red);

	FQuadTreeMeshDataThread* QuadTreeMeshDataThread;
	FRectMeshDataThread* RectMeshDataThread;
	FRectMeshDataThread* WaterMeshDataThread;
	FPoissonGeneratorThread* PoissonThread;
};

