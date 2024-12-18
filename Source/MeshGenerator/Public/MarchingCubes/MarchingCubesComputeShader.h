#pragma once

#include "CoreMinimal.h"
#include "NoiseSettings.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "MarchingCubesComputeShader.generated.h"

USTRUCT(BlueprintType)
struct MESHGENERATOR_API FMeshTriangle
{
	GENERATED_BODY()
	
	FVector3f VertexA;
	FVector3f VertexB;
	FVector3f VertexC;

	FMeshTriangle()
	{
		VertexA = FVector3f::Zero();
		VertexB = FVector3f::Zero();
		VertexC = FVector3f::Zero();
	}
};

struct MESHGENERATOR_API FMarchingCubesComputeShaderDispatchParams
{
	int X;
	int Y;
	int Z;
	
	TArray<float> Output;

	FVector3f Position;
	FVector3f Size;
	int StepSize;

	int Mode;
	int DensityFunction;

	TArray<FShaderNoiseSettings> NoiseSettings;
	int SettingsSize;
	
	FMarchingCubesComputeShaderDispatchParams(int x, int y, int z)
		: X(x)
		, Y(y)
		, Z(z)
	{
	}
};

// This is a public interface that we define so outside code can invoke our compute shader.
class MESHGENERATOR_API FMarchingCubesComputeShaderInterface {
public:

	static FMarchingCubesComputeShaderDispatchParams BuildParams(FVector3f Position, FVector3f Size, int StepSize, TArray<FNoiseSettings> NoiseSettings, TEnumAsByte<NoiseMode>
	                                                     NoiseMode = D2, TEnumAsByte<NoiseDensityFunction> DensityFunction = NoDensityFunction);
	
	// Executes this shader on the render thread
	static void DispatchRenderThread(
		FRHICommandListImmediate& RHICmdList,
		FMarchingCubesComputeShaderDispatchParams Params,
		TFunction<void(TArray<FVector4f>& Verts, TArray<FMeshTriangle>& Tris)> AsyncCallback
	);

	// static void ExecuteMultipleDispatchesAndNotify(FRHICommandListImmediate& RHICmdList, TArray<FMarchingCubesComputeShaderDispatchParams> Params, TFunction<void(TArray<TArray<float>> OutputVals)> AsyncCallback);
	// static void DispatchMyComputeShader(FRDGBuilder& GraphBuilder, FRDGBufferRef OutputBuffer, FMarchingCubesComputeShaderDispatchParams Params,int index);

	// Executes this shader on the render thread from the game thread via EnqueueRenderThreadCommand
	static void DispatchGameThread(
		FMarchingCubesComputeShaderDispatchParams Params,
		TFunction<void(TArray<FVector4f>& Verts, TArray<FMeshTriangle>& Tris)> AsyncCallback
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
		FMarchingCubesComputeShaderDispatchParams Params,
		TFunction<void(TArray<FVector4f>& Verts, TArray<FMeshTriangle>& Tris)> AsyncCallback
	)
	{
		if (IsInRenderingThread()) {
			DispatchRenderThread(GetImmediateCommandList_ForRenderCommand(), Params, AsyncCallback);
		}else{
			DispatchGameThread(Params, AsyncCallback);
		}
	}
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMarchingCubesComputeShaderLibrary_AsyncExecutionCompleted, const TArray<FVector4f>&,Verts,TArray<FMeshTriangle>&,Tris);


UCLASS(BlueprintType) // Change the _API to match your project
class MESHGENERATOR_API UMarchingCubesComputeShaderLibrary_AsyncExecution : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	
	// Execute the actual load
	virtual void Activate() override {
		FMarchingCubesComputeShaderDispatchParams Params = FMarchingCubesComputeShaderInterface::BuildParams(Position, Size, StepSize, NoiseSettings);

		// Dispatch the compute shader and wait until it completes
		FMarchingCubesComputeShaderInterface::Dispatch(Params, [this](TArray<FVector4f>& Verts, TArray<FMeshTriangle>& Tris) {
			this->Completed.Broadcast(Verts,Tris);
		});
	}
	
	UFUNCTION(BlueprintCallable, meta = (Category = "ComputeShader", WorldContext = "WorldContextObject"))
	static UMarchingCubesComputeShaderLibrary_AsyncExecution* ExecuteMarchingCubesComputeShader(UObject* WorldContextObject, FVector3f Position, FVector3f Size, int StepSize,TArray<FNoiseSettings> NoiseSettings) {
		UMarchingCubesComputeShaderLibrary_AsyncExecution* Action = NewObject<UMarchingCubesComputeShaderLibrary_AsyncExecution>();
		Action->Position = Position;
		Action->Size = Size;
		Action->StepSize = StepSize;
		Action->NoiseSettings = NoiseSettings;
		Action->RegisterWithGameInstance(WorldContextObject);

		return Action;
	}

	UPROPERTY(BlueprintAssignable)
	FOnMarchingCubesComputeShaderLibrary_AsyncExecutionCompleted Completed;

	TArray<FNoiseSettings> NoiseSettings;
	FVector3f Position;
	FVector3f Size;
	int StepSize;
	
};