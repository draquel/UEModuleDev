#pragma once

#include "CoreMinimal.h"
#include "NoiseSettings.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "NoiseComputeShader.generated.h"

struct NOISEGENERATOR_API FNoiseComputeShaderDispatchParams
{
	int X;
	int Y;
	int Z;
	
	TArray<float> Output;

	FVector3f Position;
	FVector3f Size;

	int Mode;
	FVector3f Offset;
	int Octaves;
	float Frequency;
	float Lacunarity;
	float Persistence;	
	float Scale;
	int StepSize;
	int Filter;
	int Type;
	int DensityFunction;
	int NormalizeMode;
	float DomainWarp;

	FNoiseComputeShaderDispatchParams(int x, int y, int z)
		: X(x)
		, Y(y)
		, Z(z)
	{
	}
};

// This is a public interface that we define so outside code can invoke our compute shader.
class NOISEGENERATOR_API FNoiseComputeShaderInterface {
public:

	static FNoiseComputeShaderDispatchParams BuildParams(FVector3f Position, FVector3f Size, int StepSize, FNoiseSettings NoiseSettings, TEnumAsByte<NoiseMode> Mode = D2, TEnumAsByte<NoiseDensityFunction> DensityFunction = NoDensityFunction);
	
	// Executes this shader on the render thread
	static void DispatchRenderThread(
		FRHICommandListImmediate& RHICmdList,
		FNoiseComputeShaderDispatchParams Params,
		TFunction<void(TArray<float> OutputVals)> AsyncCallback
	);

	static void ExecuteMultipleDispatchesAndNotify(FRHICommandListImmediate& RHICmdList, TArray<FNoiseComputeShaderDispatchParams> Params, TFunction<void(TArray<TArray<float>> OutputVals)> AsyncCallback);
	static void DispatchMyComputeShader(FRDGBuilder& GraphBuilder, FRDGBufferRef OutputBuffer, FNoiseComputeShaderDispatchParams Params,int index);

	// Executes this shader on the render thread from the game thread via EnqueueRenderThreadCommand
	static void DispatchGameThread(
		FNoiseComputeShaderDispatchParams Params,
		TFunction<void(TArray<float> OutputVals)> AsyncCallback
	)
	{
		ENQUEUE_RENDER_COMMAND(SceneDrawCompletion)(
		[Params, AsyncCallback](FRHICommandListImmediate& RHICmdList)
		{
			DispatchRenderThread(RHICmdList, Params, AsyncCallback);
		});
	}

	// Dispatches this shader. Can be called from any thread
	static void Dispatch(
		FNoiseComputeShaderDispatchParams Params,
		TFunction<void(TArray<float> OutputVals)> AsyncCallback
	)
	{
		if (IsInRenderingThread()) {
			DispatchRenderThread(GetImmediateCommandList_ForRenderCommand(), Params, AsyncCallback);
		}else{
			DispatchGameThread(Params, AsyncCallback);
		}
	}

	static void LayeredDispatch(TArray<FNoiseComputeShaderDispatchParams> Params, TFunction<void(TArray<TArray<float>> OutputVals)> AsyncCallback)
	{
		if (IsInRenderingThread()){
			ExecuteMultipleDispatchesAndNotify(GetImmediateCommandList_ForRenderCommand(), Params, AsyncCallback);	
		}else{
			ENQUEUE_RENDER_COMMAND(SceneDrawCompletion)(
			[Params, AsyncCallback](FRHICommandListImmediate& RHICmdList)
			{
				ExecuteMultipleDispatchesAndNotify(RHICmdList, Params, AsyncCallback);
			});
		}
		
	}
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNoiseComputeShaderLibrary_AsyncExecutionCompleted, const TArray<float>&, Values);


UCLASS(BlueprintType, Blueprintable) // Change the _API to match your project
class NOISEGENERATOR_API UNoiseComputeShaderLibrary_AsyncExecution : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	
	// Execute the actual load
	virtual void Activate() override {
		FNoiseComputeShaderDispatchParams Params = FNoiseComputeShaderInterface::BuildParams(Position, Size, StepSize, NoiseSettings);

		// Dispatch the compute shader and wait until it completes
		FNoiseComputeShaderInterface::Dispatch(Params, [this](TArray<float> OutputVals) {
			this->Completed.Broadcast(OutputVals);
		});
	}
	
	UFUNCTION(BlueprintCallable, meta = (Category = "ComputeShader", WorldContext = "WorldContextObject"))
	static UNoiseComputeShaderLibrary_AsyncExecution* ExecuteNoiseComputeShader(UObject* WorldContextObject, FVector3f Position, FVector3f Size, int StepSize, FNoiseSettings NoiseSettings) {
		UNoiseComputeShaderLibrary_AsyncExecution* Action = NewObject<UNoiseComputeShaderLibrary_AsyncExecution>();
		Action->Position = Position;
		Action->Size = Size;
		Action->StepSize = StepSize;
		Action->NoiseSettings = NoiseSettings;
		Action->RegisterWithGameInstance(WorldContextObject);

		return Action;
	}

	static UNoiseComputeShaderLibrary_AsyncExecution* ExecuteNoiseComputeShader( FVector3f Position, FVector3f Size, int StepSize, FNoiseSettings NoiseSettings) {
		UNoiseComputeShaderLibrary_AsyncExecution* Action = NewObject<UNoiseComputeShaderLibrary_AsyncExecution>();
		Action->Position = Position;
		Action->Size = Size;
		Action->StepSize = StepSize;
		Action->NoiseSettings = NoiseSettings;

		return Action;
	}

	UPROPERTY(BlueprintAssignable)
	FOnNoiseComputeShaderLibrary_AsyncExecutionCompleted Completed;

	FNoiseSettings NoiseSettings;
	FVector3f Position;
	FVector3f Size;
	int StepSize;
	
};