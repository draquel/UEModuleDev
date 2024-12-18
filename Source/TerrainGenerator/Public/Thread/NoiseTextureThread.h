// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TerrainChunk.h"

struct FNoiseSettings;
/**
 * 
 */
class TERRAINGENERATOR_API FNoiseTextureThread : public FRunnable
{
public:
	FNoiseTextureThread(ATerrainChunk* Chunk, FVector PlayerPos, FIntVector2 Coord, FIntVector Position, FIntVector2 Size, int32 StepSize, TArray<FNoiseSettings>* NoiseSettings)
	{
		this->Chunk = Chunk;
		this->PlayerPos = PlayerPos;
		this->Coord = Coord;
		this->Position = Position;
		this->Size = Size;
		this->StepSize = StepSize;
		this->NoiseSettings = NoiseSettings;

		Thread = FRunnableThread::Create(this, TEXT("MyThread"));
	}
	
	virtual ~FNoiseTextureThread() override
	{
		if(Thread)
		{
			Thread->Kill();
			delete Thread;
		}
	}

	ATerrainChunk* Chunk;
	FVector PlayerPos;
	FIntVector2 Coord;
	FIntVector Position;
	FIntVector2 Size;
	int32 StepSize;
	TArray<FNoiseSettings>* NoiseSettings;
	int heightMultiplier;
	bool Collision = false;
	int Section = false;
	
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;
	virtual void Stop() override;

	FRunnableThread* Thread;
	bool bShutdown = false;
	bool bDataReady = false;
	double start = FPlatformTime::Seconds();
};
