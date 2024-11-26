#pragma once
#include "MeshData/MeshData.h"

class ATerrainChunk;
struct FNoiseSettings;

class MESHGENERATOR_API FRectMeshDataThread : public FRunnable
{
public:
	FRectMeshDataThread(UProceduralMeshComponent* component, FVector position, FVector size, FVector2D segments, float uvScale,int section, bool collision)
	{
		Component = component;
		Position = position;
		Size = size;
		Segments = segments;
		UVScale = uvScale;
		Section = section;
		Collision = collision;
		
		Thread = FRunnableThread::Create(this, TEXT("MyThread"));
	}

	FRectMeshDataThread(UProceduralMeshComponent* component, FVector position, FVector size, FVector2D segments, float uvScale, int section, bool collision, FNoiseSettings* noiseSettings, int heightMultiplier)
	{
		Component = component;
		Position = position;
		Size = size;
		Segments = segments;
		UVScale = uvScale;

		NoiseSettings = noiseSettings;
		HeightMultiplier = heightMultiplier;
		Section = section;
		Collision = collision;
		withNoise = true;
		
		Thread = FRunnableThread::Create(this, TEXT("MyThread"));
	}

	virtual ~FRectMeshDataThread()
	{
		if(Thread)
		{
			Thread->Kill();
			delete Thread;
		}
	}
	
	FMeshData MeshData;

	UProceduralMeshComponent* Component;
	FVector Position;
	FVector Size;
	FVector2D Segments;
	float UVScale;
	FNoiseSettings* NoiseSettings;
	int HeightMultiplier;
	int Section = 0;
	bool Collision = false;
	bool withNoise = false;
	
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;
	virtual void Stop() override;

	FRunnableThread* Thread;
	bool bShutdown = false;
	bool bDataReady = false;
	double start = FPlatformTime::Seconds();
};
