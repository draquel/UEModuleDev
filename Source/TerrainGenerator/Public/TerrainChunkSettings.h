#pragma once
#include "CoreMinimal.h"
#include "NoiseSettings.h"
#include "QuadTree/QuadTreeSettings.h"
#include "TerrainChunkSettings.generated.h"

/*
 * FoliageGroupSettings - Stores the options for meshes and generating points as well as the list of meshes to instance.
 */
USTRUCT(BlueprintType)
struct FFoliageGroupSettings
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere)
	FName Name;

	UPROPERTY(EditAnywhere)
	int32 FoliageMinDist = 1000;

	UPROPERTY(EditAnywhere)
	int32 LoadRange = -1;

	UPROPERTY(EditAnywhere)
	int32 FoliageNewPoints = 3;

	UPROPERTY(EditAnywhere)
	FVector FoliageScale = FVector::One();

	UPROPERTY(EditAnywhere)
	FVector FoliageOffset = FVector::Zero();

	UPROPERTY(EditAnywhere)
	TArray<UStaticMesh*> Foliage;

	UPROPERTY(EditAnywhere)
	bool EnableCollision = false;
};

/*
 * FoliageGroupData - Stores the generated points and mesh instances
 */
USTRUCT(BlueprintType)
struct FFoliageGroupData
{
	GENERATED_BODY();

	FName Name;
	
	TArray<FVector2d> FoliagePoints;

	TArray<UInstancedStaticMeshComponent*> FoliageInstances;

	bool hasFoliage = false;	
};

/*
 *	TerrainNoiseSettings - Stores NoiseSettings for each of the terrain noise maps
 */
// USTRUCT()
// struct FTerrainNoiseSettings
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY(EditAnywhere)
// 	FLayeredNoiseSettings Continentalness;
// 	
// 	UPROPERTY(EditAnywhere)
// 	FLayeredNoiseSettings Errosion;
//
// 	UPROPERTY(EditAnywhere)
// 	FLayeredNoiseSettings PeaksVallies;
//
// 	TArray<FLayeredNoiseSettings> ToTArray()
// 	{
// 		return { Continentalness, Errosion, PeaksVallies };
// 	}
// };

/*
 * TerrainChunkSettings - Configuration for instancing consistent TerrainChunks.
 */
USTRUCT(BlueprintType)
struct FTerrainChunkSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FVector Size = FVector(6400,6400,2000);

	UPROPERTY(EditAnywhere)
	int32 WaterResolution = 20;

	UPROPERTY(EditAnywhere, meta=(ClampMin=0))
	int32 DepthFilter = 0;
	
	UPROPERTY(EditAnywhere, meta=(ClampMin = 0.0001f))
	float UVScale = 0.01;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* Material;

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0.0001f))
	float WaterUVScale = 0.1;
	
	UPROPERTY(EditAnywhere)
	UMaterialInterface* WaterMaterial;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* WaterCausticsMaterial;	

	UPROPERTY(EditAnywhere)
	bool EnableWater = true;
	
	UPROPERTY(EditAnywhere)
	bool EnableFoliage = true;

	UPROPERTY(EditAnywhere)
	TArray<FFoliageGroupSettings> FoliageGroups;
	
	UPROPERTY(EditAnywhere)
	FNoiseSettings NoiseSettings;

	// UPROPERTY(EditAnywhere)
	// FTerrainNoiseSettings TerrainNoiseSettings;

	UPROPERTY(EditAnywhere)
	int32 MeshResolution = 64;

	UPROPERTY(EditAnywhere)
	bool QuadtreeMeshEnabled = true;
	
	UPROPERTY(EditAnywhere)
	FQuadTreeSettings QuadTreeSettings;

	UPROPERTY(EditAnywhere)
	bool EnableDebugDraw = false;

	FTerrainChunkSettings()
	{
		Material = nullptr;
		WaterMaterial = nullptr;
		WaterCausticsMaterial = nullptr;
	}
};


