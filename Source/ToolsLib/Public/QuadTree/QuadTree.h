#pragma once

#include "QuadTreeNode.h"
#include "QuadTree/QuadTreeSettings.h"

struct FNoiseSettings;
class QuadTreeNode;

class TOOLSLIB_API QuadTree
{
public:
	QuadTree();
	QuadTree(FVector position, FVector size, FQuadTreeSettings settings);
	QuadTreeNode RootNode;

	TArray<QuadTreeNode> Leaves;

	FVector Position;
	FVector Center;
	FVector Size;

	FQuadTreeSettings Settings;
	
	void GenerateTree(FVector viewerPosition);
	int GetDepth();
	void UpdateSettings(FQuadTreeSettings settings);
};
