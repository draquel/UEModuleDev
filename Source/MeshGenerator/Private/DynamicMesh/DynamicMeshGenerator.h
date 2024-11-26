#pragma once
#include "CoreMinimal.h"
#include "UDynamicMesh.h"
#include "NoiseSettings.h"
#include "QuadTree/QuadTree.h"

class MESHGENERATOR_API DynamicMeshGenerator 
{
public:
	static FDynamicMesh3 RectMesh(FVector position, FVector size, FVector2D segments, float UVScale = 1, FNoiseSettings* NoiseSettings = NULL, int heightMultiplier = 500);
	static FDynamicMesh3 QuadTreeMesh(QuadTree* QTree, FVector playerPos, float UVScale, int depthFilter, FNoiseSettings* NoiseSettings, int heightMultiplier);
	static void AverageVertexNormalsByUVPosition(FDynamicMesh3& Mesh);
};
