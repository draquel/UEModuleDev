#pragma once
#include "CoreMinimal.h"
#include "MeshData/MeshData.h"

struct FNoiseSettings;
class QuadTree;

class MESHGENERATOR_API FQuadTreeMeshDataThread : public FRunnable {

public:
	FQuadTreeMeshDataThread(UProceduralMeshComponent* component, QuadTree* qtree, float uvScale, int depthFilter,int section, bool collision, FNoiseSettings* noiseSettings, int heightMultiplier) {
		Component = component;
		QTree = qtree;
		UVScale = uvScale;
		this->depthFilter = depthFilter;
		NoiseSettings = noiseSettings;
		this->heightMultiplier = heightMultiplier;
		Section = section;
		Collision = collision;
		
		Thread = FRunnableThread::Create(this, TEXT("MyThread"));
	};

	virtual ~FQuadTreeMeshDataThread()
	{
		if(Thread)
		{
			Thread->Kill();
			delete Thread;
		}
	}

	UProceduralMeshComponent* Component;
	QuadTree* QTree;
	float UVScale;
	int depthFilter;
	FNoiseSettings* NoiseSettings;
	int heightMultiplier;
	bool Collision = false;
	int Section = false;

	FMeshData MeshData;

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;
	virtual void Stop() override;

	FRunnableThread* Thread;
	bool bShutdown = false;
	bool bDataReady = false;
	double start = FPlatformTime::Seconds();
};
