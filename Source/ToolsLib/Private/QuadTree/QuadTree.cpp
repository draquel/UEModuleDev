#include "QuadTree/QuadTree.h"

QuadTree::QuadTree()
{
	
}

QuadTree::QuadTree(FVector position, FVector size, FQuadTreeSettings settings)
{
	Position = position;
	Size = size;

	Settings = settings;
	Center = Position + (size / 2);
}

void QuadTree::GenerateTree(FVector viewerPosition)
{
	double start = FPlatformTime::Seconds();
	Leaves.Reset();
	RootNode = QuadTreeNode(Center,Size,&RootNode,&Settings);
	RootNode.GenerateNode(&Leaves,viewerPosition);
	for(int i = 0; i < Leaves.Num(); i++) {
		Leaves[i].CheckNeighbors();
	}
	double end = FPlatformTime::Seconds();
	//UE_LOG(LogTemp,Log,TEXT("QuadTree::GenerateTree() => Leaves: %d, Depth: %d, Duration: %f s"),Leaves.Num(),GetDepth(),end-start);
}

int QuadTree::GetDepth()
{
	int depth = 0;
	for(int i = 0; i < Leaves.Num(); i++) {
		if(depth < Leaves[i].Depth)	{
			depth = Leaves[i].Depth;
		}
	}
	return depth;
}

void QuadTree::UpdateSettings(FQuadTreeSettings settings)
{
	Settings = settings;
}