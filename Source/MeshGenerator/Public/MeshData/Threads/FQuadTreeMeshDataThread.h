#pragma once
#include "CoreMinimal.h"
#include "MeshData/MeshData.h"

struct FNoiseSettings;
class UQuadTree;

class MESHGENERATOR_API FQuadTreeMeshDataThread : public FRunnable {

public:
	FQuadTreeMeshDataThread(UProceduralMeshComponent* component, UQuadTree* qtree, float uvScale, int depthFilter,int section, bool collision, UTexture2D* NoiseTexture, int heightMultiplier) {
		Component = component;
		QTree = qtree;
		UVScale = uvScale;
		this->depthFilter = depthFilter;
		this->NoiseTexture = NoiseTexture;
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
	UQuadTree* QTree;
	float UVScale;
	int depthFilter;
	FNoiseSettings* NoiseSettings;
	UTexture2D* NoiseTexture;
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
