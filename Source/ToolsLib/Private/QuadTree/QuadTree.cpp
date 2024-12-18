#include "QuadTree/QuadTree.h"

#include "ToolsLib.h"

UQuadTree::UQuadTree()
{
	
}

UQuadTree::UQuadTree(FVector position, FVector size, FQuadTreeSettings settings)
{
	Position = position;
	Size = size;

	Settings = settings;
	Center = Position + (size / 2);
}

void UQuadTree::Init(FVector position, FVector size, FQuadTreeSettings settings)
{
	Position = position;
	Size = size;
	Settings = settings;
}

void UQuadTree::GenerateTree(FVector viewerPosition)
{
	double start = FPlatformTime::Seconds();
	Leaves.Reset();
	RootNode = QuadTreeNode(Center,Size,&RootNode,&Settings);
	RootNode.GenerateNode(&Leaves,viewerPosition);
	for(int i = 0; i < Leaves.Num(); i++) {
		Leaves[i].CheckNeighbors();
	}
	double end = FPlatformTime::Seconds();
	UE_LOG(QuadTree,Log,TEXT("QuadTree::GenerateTree() ==> Leaves: %d, Depth: %d, RunTime: %f s"),Leaves.Num(),GetDepth(),end-start);
}

int UQuadTree::GetDepth()
{
	int depth = 0;
	for(int i = 0; i < Leaves.Num(); i++) {
		if(depth < Leaves[i].Depth)	{
			depth = Leaves[i].Depth;
		}
	}
	return depth;
}

void UQuadTree::UpdateSettings(FQuadTreeSettings settings)
{
	Settings = settings;
}

int UQuadTree::CountVerts(TArray<QuadTreeNode> nodes)
{
	int count = 0;
	for (int i = 0; i < nodes.Num(); i++) {
		int nCount = 0;
		nCount += nodes[i].Neighbors[0] ? 1 : 0;
		nCount += nodes[i].Neighbors[1] ? 1 : 0;
		nCount += nodes[i].Neighbors[2] ? 1 : 0;
		nCount += nodes[i].Neighbors[3] ? 1 : 0;
		count += 9 - nCount;
	}
	return count;
}
