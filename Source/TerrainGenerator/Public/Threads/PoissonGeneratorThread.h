#pragma once
#include "TerrainChunk.h"


class TERRAINGENERATOR_API FPoissonGeneratorThread : public FRunnable
{
public:
	FPoissonGeneratorThread(ATerrainChunk* chunk, FFoliageGroupSettings* settings, FFoliageGroupData* data, const FVector2D topLeft, const FVector2D bottomRight, float minDist, int newPointsCount)
	{
		Chunk = chunk;
		Settings = settings;
		Data = data;
		TopLeft = topLeft;
		BottomRight = bottomRight;
		MinDist = minDist;
		NewPointsCount = newPointsCount;
		Thread = FRunnableThread::Create(this, TEXT("MyThread"));
	}

	virtual ~FPoissonGeneratorThread()
	{
		if(Thread) {
			Thread->Kill();
			delete Thread;
		}
	}
	
	FRunnableThread* Thread;
	ATerrainChunk* Chunk;
	FFoliageGroupSettings* Settings;
	FFoliageGroupData* Data;

	FVector2D TopLeft;
	FVector2D BottomRight;
	float MinDist;
	int NewPointsCount;

	TArray<FVector2D> Points;
	
	bool bShutdown = false;
	bool bDataReady = false;

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;
	virtual void Stop() override;
};
