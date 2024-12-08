#pragma once

#include "Noise.h"
#include "MeshData/Threads/FQuadTreeMeshDataThread.h"
#include "MeshData/Threads/FRectMeshDataThread.h"
#include "MeshDataGenerator.generated.h"

struct FRectMeshDataThreadData;
struct FQuadTreeMeshDataThreadData;
struct FNoiseSettings;
struct FMeshData;
class QuadTree;

UCLASS(BlueprintInternalUseOnly)
class MESHGENERATOR_API	UMeshDataGenerator : public UObject
{
	GENERATED_BODY()
public:

	static TQueue<FQuadTreeMeshDataThreadData*,EQueueMode::Mpsc> QuadTreeMeshDataThread;
	static TQueue<FRectMeshDataThreadData*,EQueueMode::Mpsc> RectMeshDataThread;

	static FMeshData RectMesh(FVector position, FVector size, FVector2D segments, float UVScale = 1, FNoiseSettings* NoiseSettings = NULL, int heightMultiplier = 500);
	static FMeshData QuadTreeMesh(QuadTree* QTree, float UVScale = 1, int depthFilter = 0, FNoiseSettings* NoiseSettings = NULL, int heightMultiplier = 500);
	static FMeshData MarchingCubes(FNoiseMap3d NoiseMap, FVector position, FVector size, float UVScale, float isoLevel, bool interpolate, bool renderSides = false);

private:
	static FVector Interp(FVector edgeVertex1, float valueAtVertex1, FVector edgeVertex2, float valueAtVertex2, float isoLevel);
	static FVector Default(FVector edgeVertex1, FVector edgeVertex2);
};

struct FQuadTreeMeshDataThreadData
{
public:
	FQuadTreeMeshDataThreadData(FQuadTreeMeshDataThread* thread, ATerrainChunk* chunk)
	{
		Thread = thread;
		Chunk = chunk;
	}
	
	FQuadTreeMeshDataThread* Thread;
	ATerrainChunk* Chunk;
};

struct FRectMeshDataThreadData
{
public:
	FRectMeshDataThreadData(FRectMeshDataThread* thread, ATerrainChunk* chunk)
	{
		Thread = thread;
		Chunk = chunk;
	}
	
	FRectMeshDataThread* Thread;
	ATerrainChunk* Chunk;
};