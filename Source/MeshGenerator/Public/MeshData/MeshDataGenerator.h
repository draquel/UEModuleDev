#pragma once

#include "Noise.h"
#include "MarchingCubes/MarchingCubes.h"
#include "MeshData/Threads/FQuadTreeMeshDataThread.h"
#include "MeshData/Threads/FRectMeshDataThread.h"
#include "MeshDataGenerator.generated.h"

struct FRectMeshDataThreadData;
struct FQuadTreeMeshDataThreadData;
struct FNoiseSettings;
struct FMeshData;
class UQuadTree;

UCLASS(BlueprintInternalUseOnly)
class MESHGENERATOR_API	UMeshDataGenerator : public UObject
{
	GENERATED_BODY()
public:

	static TQueue<FQuadTreeMeshDataThreadData*,EQueueMode::Mpsc> QuadTreeMeshDataThread;
	static TQueue<FRectMeshDataThreadData*,EQueueMode::Mpsc> RectMeshDataThread;

	static FMeshData RectMesh(FVector position, FVector size, FVector2D segments, float UVScale = 1, FNoiseSettings* NoiseSettings = NULL, int heightMultiplier = 500);
	static FMeshData RectMesh(FNoiseMap2d* NoiseMap, FVector position, FVector size, float UVScale, int heightMultiplier);
	static FMeshData RectMesh(UTexture2D* Texture, FVector position, FVector size, int StepSize, int TextureStepSize, float UVScale, int
	                          heightMultiplier);

	static FMeshData QuadTreeMesh(UQuadTree* QTree, float UVScale = 1, int depthFilter = 0, FNoiseSettings* NoiseSettings = NULL, int heightMultiplier = 500);
	static FMeshData QuadTreeMesh(FNoiseMap2d* NoiseMap, UQuadTree* QTree, float UVScale, int depthFilter, int heightMultiplier);
	static FMeshData QuadTreeMesh(UTexture2D* Texture, UQuadTree* QTree, int StepSize, float UVScale, int depthFilter, int heightMultiplier);
	// static FMeshData QuadTreeMesh(UTextureRenderTarget2D* RenderTarget, UQuadTree* QTree, int StepSize, float UVScale, int depthFilter, int heightMultiplier);
	
	static FMeshData MarchingCubes(FVector position, FVector size, int stepSize, FNoiseSettings* NoiseSettings, float UVScale, FMarchingCubesSettings MarchingCubesSettings);
	static FMeshData MarchingCubes(FNoiseMap3d* NoiseMap, FVector position, FVector size, int stepSize, float UVScale, FMarchingCubesSettings MarchingCubesSettings);

private:
	static FVector Interp(FVector edgeVertex1, float valueAtVertex1, FVector edgeVertex2, float valueAtVertex2, float isoLevel);
	static FVector Default(FVector edgeVertex1, FVector edgeVertex2);
	static float SampleTexture(UTexture2D* Texture, FIntVector2 Coordinates);
	// static float SampleRenderTargetR16f(UTextureRenderTarget2D* RenderTarget, FIntVector2 Coordinates);
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