// Fill out your copyright notice in the Description page of Project Settings.


#include "Thread/NoiseTextureThread.h"
#include "Noise.h"

bool FNoiseTextureThread::Init() {
	return true;
}

uint32 FNoiseTextureThread::Run() {
	// MeshData = UMeshDataGenerator::QuadTreeMesh(QTree,UVScale,depthFilter,NoiseSettings,heightMultiplier);
	// MeshData = UMeshDataGenerator::QuadTreeMesh(NoiseTexture,QTree,100,UVScale,depthFilter,heightMultiplier);
	
	// UNoise::GenerateTexture(RT,Position,Size,StepSize,NoiseSettings,[this]() {
	// 	UE_LOG(LogTemp,Log,TEXT("FNoiseTextureThread::Run = Chunk (%i,%i) Noise"),Coord.X,Coord.Y);
	// 	Texture = RT->ConstructTexture2D(World, FString::Printf(TEXT("Chunk (%i,%i) Noise"),Coord.X,Coord.Y), RF_Public | RF_Standalone); 
	// 	bDataReady = true;	
	// });

	UNoise::GenerateTexture(Chunk->NoiseTexture,Position,Size,StepSize,*NoiseSettings, [this](UTexture2D* NoiseTexture)
	{
		Chunk->NoiseTexture = NoiseTexture;
		bDataReady = true;	
	});
	
	//UE_LOG(LogTemp,Log,TEXT("FNoiseTextureThread:Run() => Verticies: %d, Triangles: %d"),MeshData.Vertices.Num(),MeshData.Triangles.Num());
	return 0;
}

void FNoiseTextureThread::Exit()
{
	AsyncTask(ENamedThreads::GameThread, [this] ()
	{
		Chunk->CreateTerrainMesh(PlayerPos,true);
		if(Chunk->Settings.EnableWater){ if(!Chunk->hasWater){ Chunk->CreateWaterMesh(); }  } else { Chunk->WaterMesh->ClearMeshSection(0); Chunk->hasWater = false; }
		if(Chunk->Settings.EnableFoliage) { Chunk->CreateFoliage(PlayerPos, true); }
		Chunk->DebugDraw(0.75f,FColor::Orange);		
	}); 
}

void FNoiseTextureThread::Stop() {
	bShutdown = true;
}
