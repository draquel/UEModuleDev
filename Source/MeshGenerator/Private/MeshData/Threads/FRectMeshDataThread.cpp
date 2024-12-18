#include "MeshData/Threads/FRectMeshDataThread.h"

#include "AI/NavigationSystemBase.h"
#include "MeshData/MeshDataGenerator.h"

bool FRectMeshDataThread::Init() {
	return true;
}

uint32 FRectMeshDataThread::Run() {
	if(withNoise) {
		MeshData = UMeshDataGenerator::RectMesh(Position, Size, Segments, UVScale,NoiseSettings,HeightMultiplier);
	} else {
		MeshData = UMeshDataGenerator::RectMesh(Position, Size, Segments, UVScale,nullptr,0);
	}
	bDataReady = true;
	//UE_LOG(LogTemp,Log,TEXT("FRectMeshDataThread:Run() => Verticies: %d, Triangles: %d"),MeshData.Vertices.Num(),MeshData.Triangles.Num());
	return 0;
}

void FRectMeshDataThread::Exit() {
	AsyncTask(ENamedThreads::GameThread, [this] ()
	{
		if(Component == nullptr){ return; }
		
		MeshData.CreateProceduralMesh(Component,Section,Collision);
		
		if(Collision) {
			MeshData.ConfigureForNavigation(Component);
		}
	});
}

void FRectMeshDataThread::Stop() {
	bShutdown = true;
}
