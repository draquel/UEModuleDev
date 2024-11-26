#include "MeshData/Threads/FQuadTreeMeshDataThread.h"
#include "MeshData/MeshDataGenerator.h"

bool FQuadTreeMeshDataThread::Init() {
	return true;
}

uint32 FQuadTreeMeshDataThread::Run() {
	MeshData = UMeshDataGenerator::QuadTreeMesh(QTree,UVScale,depthFilter,NoiseSettings,heightMultiplier);
	bDataReady = true;
	//UE_LOG(LogTemp,Log,TEXT("FQuadTreeMeshDataThread:Run() => Verticies: %d, Triangles: %d"),MeshData.Vertices.Num(),MeshData.Triangles.Num());
	return 0;
}

void FQuadTreeMeshDataThread::Exit()
{
	AsyncTask(ENamedThreads::GameThread, [this] ()
	{
		if(Component == nullptr){ return; }
		
		MeshData.CreateProceduralMesh(Component,Section,Collision);
		
		if(Collision) {
			MeshData.ConfigureForNavigation(Component);
		}
	}); 
}

void FQuadTreeMeshDataThread::Stop() {
	bShutdown = true;
}
