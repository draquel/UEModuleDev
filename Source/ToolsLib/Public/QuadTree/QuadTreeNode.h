#pragma once

#include "QuadTreeSettings.h"
#include "QuadTreeNode.generated.h"

class TOOLSLIB_API QuadTreeNode
{
public:
	FVector Center;
	FVector Size;

	UINT32 Hash;
	int Depth;
	int Corner;

	FQuadTreeSettings* Settings;

	QuadTreeNode* RootNode;

	bool Neighbors[4] = {true,true,true,true};
	TArray<QuadTreeNode> Children;

	QuadTreeNode();
	QuadTreeNode(FVector center, FVector size, QuadTreeNode* rootNode, FQuadTreeSettings* settings,UINT32 hash = 1, int depth = 0, int corner = 0);

	void GenerateNode(TArray<QuadTreeNode>* leaves, FVector viewerPosition);
	void GenerateChildren(TArray<QuadTreeNode>* leaves, FVector viewerPosition);

	bool DistanceCheck(FVector viewerPosition);
	bool NodeCheck();
	void Split();

	void CheckNeighbors();
	bool CheckNeighborDepth(int direction, UINT hash) const;
	int GetNeighborDepth(UINT queryHash, int targetDepth);

};

USTRUCT()
struct TOOLSLIB_API FQuadTreeLeaf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FVector3f Center;
	UPROPERTY(EditAnywhere)
	FVector3f Size;
	UPROPERTY(EditAnywhere)
	uint32 Depth;
	UPROPERTY(EditAnywhere)
	FVector4f Neighbors;

	FQuadTreeLeaf()
	{
		Center = FVector3f::Zero();
		Size = FVector3f::Zero();
		Depth = 0;
		Neighbors = FVector4f::Zero();
	}

	FQuadTreeLeaf(QuadTreeNode Node)
	{
		Center = (FVector3f)Node.Center;
		Size = (FVector3f)Node.Size;
		Depth = Node.Depth;
		Neighbors = FVector4f(Node.Neighbors[0]?1:0,Node.Neighbors[1]?1:0,Node.Neighbors[2]?1:0,Node.Neighbors[3]?1:0);
	}
};