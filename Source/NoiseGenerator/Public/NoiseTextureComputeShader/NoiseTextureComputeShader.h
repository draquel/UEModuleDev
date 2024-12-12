#include "CoreMinimal.h"
#include "NoiseSettings.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "NoiseTextureComputeShader.generated.h"

struct NOISEGENERATOR_API FNoiseTextureComputeShaderDispatchParams
{
	int X;
	int Y;
	int Z;
	
	FRenderTarget* RenderTarget;

	FVector3f Position;
	FVector3f Size;

	// int Mode;
	FVector3f Offset;
	int Octaves;
	float Frequency;
	float Lacunarity;
	float Persistence;	
	float Scale;
	int StepSize;
	int Filter;
	int Type;
	// int DensityFunction;
	int NormalizeMode;
	float DomainWarp;
	
	FNoiseTextureComputeShaderDispatchParams(int x, int y, int z)
		: X(x)
		, Y(y)
		, Z(z)
	{
	}
};

// This is a public interface that we define so outside code can invoke our compute shader.
class NOISEGENERATOR_API FNoiseTextureComputeShaderInterface {
public:
	static FNoiseTextureComputeShaderDispatchParams BuildParams(UTextureRenderTarget2D* RenderTarget, FVector3f Position, FVector3f Size, int StepSize, FNoiseSettings NoiseSettings);
	// Executes this shader on the render thread
	static void DispatchRenderThread(
		FRHICommandListImmediate& RHICmdList,
		FNoiseTextureComputeShaderDispatchParams Params
	);

	// Executes this shader on the render thread from the game thread via EnqueueRenderThreadCommand
	static void DispatchGameThread(
	FNoiseTextureComputeShaderDispatchParams Params
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
		FNoiseTextureComputeShaderDispatchParams Params,
		TFunction<void()> AsyncCallback = nullptr
	)
	{
		if (IsInRenderingThread()) {
			DispatchRenderThread(GetImmediateCommandList_ForRenderCommand(), Params);
		}else{
			DispatchGameThread(Params);
		}
		if (AsyncCallback != nullptr) {
			ENQUEUE_RENDER_COMMAND(RenderCallback)([AsyncCallback](FRHICommandListImmediate& RHICmdList) {
				AsyncTask(ENamedThreads::GameThread,AsyncCallback);
			});	
		}
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNoiseTextureComputeShaderLibrary_AsyncExecutionCompleted, const TArray<float>&, Values);

// This is a static blueprint library that can be used to invoke our compute shader from blueprints.
UCLASS(BlueprintType, Blueprintable)
class NOISEGENERATOR_API UNoiseTextureComputeShaderLibrary_AsyncExecution :  public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable)
	static void ExecuteNoiseTextureComputeShader(UTextureRenderTarget2D* RT, FVector3f Position, FVector3f Size, int StepSize, FNoiseSettings NoiseSettings)
	{
		FNoiseTextureComputeShaderDispatchParams Params = FNoiseTextureComputeShaderInterface::BuildParams(RT,Position,Size,StepSize,NoiseSettings);
		FNoiseTextureComputeShaderInterface::Dispatch(Params);
	}
};
