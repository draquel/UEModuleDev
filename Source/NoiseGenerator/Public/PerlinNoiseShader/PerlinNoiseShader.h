#pragma once

#include "CoreMinimal.h"
#include "NoiseSettings.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Materials/MaterialRenderProxy.h"

#include "PerlinNoiseShader.generated.h"


struct NOISEGENERATOR_API FPerlinNoiseShaderDispatchParams
{
	int X;
	int Y;
	int Z;

	// FNoiseSettings Settings;
	FRenderTarget* RenderTarget;
	FVector3f Position;
	FVector3f Offset;
	int Octaves;
	float Lacunarity;
	float Persistence;	
	float Scale;	

	FPerlinNoiseShaderDispatchParams(int x, int y, int z)
		: X(x)
		, Y(y)
		, Z(z)
	{
	}
};

// This is a public interface that we define so outside code can invoke our compute shader.
class NOISEGENERATOR_API FPerlinNoiseShaderInterface {
public:
	// Executes this shader on the render thread
	static void DispatchRenderThread(
		FRHICommandListImmediate& RHICmdList,
		FPerlinNoiseShaderDispatchParams Params
	);

	// Executes this shader on the render thread from the game thread via EnqueueRenderThreadCommand
	static void DispatchGameThread(
		FPerlinNoiseShaderDispatchParams Params
	)
	{
		ENQUEUE_RENDER_COMMAND(SceneDrawCompletion)(
		[Params](FRHICommandListImmediate& RHICmdList)
		{
			DispatchRenderThread(RHICmdList, Params);
		});
	}

	// Dispatches this shader. Can be called from any thread
	static void Dispatch(
		FPerlinNoiseShaderDispatchParams Params
	)
	{
		if (IsInRenderingThread()) {
			DispatchRenderThread(GetImmediateCommandList_ForRenderCommand(), Params);
		}else{
			DispatchGameThread(Params);
		}
	}
};

// This is a static blueprint library that can be used to invoke our compute shader from blueprints.
UCLASS()
class NOISEGENERATOR_API UPerlinNoiseShaderLibrary : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	static void ExecuteRTComputeShader(UTextureRenderTarget2D* RT, FVector3f Position, FVector3f Offset, int octaves, float lacunarity, float persistence, float scale)
	{
		// Create a dispatch parameters struct and fill it the input array with our args
		FPerlinNoiseShaderDispatchParams Params(RT->SizeX, RT->SizeY, 1);
		Params.RenderTarget = RT->GameThread_GetRenderTargetResource();

		Params.Position = Position;
		Params.Offset = Offset;
		Params.Octaves = octaves;
		Params.Lacunarity = lacunarity;
		Params.Persistence = persistence;
		Params.Scale = scale;

		FPerlinNoiseShaderInterface::Dispatch(Params);
	}

	// UFUNCTION(BlueprintCallable)
	// static void ExecutePerlinNosieComputeShader(UTextureRenderTarget2D* RT, FVector3f Position, FNoiseSettings NoiseSettings)
	// {
	// 	// Create a dispatch parameters struct and fill it the input array with our args
	// 	FPerlinNoiseShaderDispatchParams Params(RT->SizeX, RT->SizeY, 1);
	// 	Params.RenderTarget = RT->GameThread_GetRenderTargetResource();
	// 	Params.Settings = NoiseSettings;
	// 	Params.Position = Position;
	//
	// 	FPerlinNoiseShaderInterface::Dispatch(Params);
	// }
};
