#pragma once

struct FQuadTreeSettings;

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
