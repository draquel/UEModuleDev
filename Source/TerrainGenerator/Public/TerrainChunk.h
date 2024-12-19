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
class FNoiseTextureThread;

USTRUCT()
struct FTerrainEvaluationData
{
	GENERATED_BODY();
	
	bool valid;
	float height;
	float slope;
	FRotator up;
};

UCLASS()
class TERRAINGENERATOR_API ATerrainChunk : public AActor
{
	GENERATED_BODY()
	
public:	
	ATerrainChunk();

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
	
	UPROPERTY(EditAnywhere)
	UTexture2D* NoiseTexture;

	UPROPERTY(EditAnywhere)
	FMinMax NoiseMinMax;
	
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
	void GenerateNoise(FVector playerPos, bool allowThread);

	void CreateTerrainMesh(FVector playerPos, bool allowThread = true);
	void CreateQuadtreeMesh(FVector playerPos, bool allowThread = true);
	void CreateRectMesh(bool allowThread = true);
	void CreateWaterMesh(bool allowThread = true);
	void CreateFoliage(FVector playerPos, bool allowThread, bool regenerate = false);
	void InstanceFoliage(TArray<UInstancedStaticMeshComponent*>* StaticMeshComponents, TArray<UStaticMesh*>* Foliage, TArray<FVector2D>* Points, FVector Offset, FVector Scale, FMinMax ElevationMinMax, FMinMax SlopeMinMax, bool RotateToTerrain, bool Collision);
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
	
	UQuadTree QTree;
	
	float EvaluateSlope(UTexture2D* Texture, FVector pos, int heightMultiplier);
	FRotator EvaluateUp(UTexture2D* Texture, FVector pos, int heightMultiplier);
	bool EvaluateTerrain(UTexture2D* Texture, FVector pos, int heightMultiplier, FVector2D elevationLimits, FVector2D slopeLimits, FTerrainEvaluationData* data);

	void DebugDrawBounds(float life = 30, FColor color = FColor::Red);
	void DebugDrawPositionMarker(float life = 30, FColor color = FColor::Red);

	FNoiseTextureThread* NoiseTextureThread;
	FQuadTreeMeshDataThread* QuadTreeMeshDataThread;
	FRectMeshDataThread* RectMeshDataThread;
	FRectMeshDataThread* WaterMeshDataThread;
	FPoissonGeneratorThread* PoissonThread;
};

