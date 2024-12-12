#include "NoiseComputeShader.h"
#include "NoiseGenerator/Public/NoiseComputeShader/NoiseComputeShader.h"
#include "PixelShaderUtils.h"
#include "MeshPassProcessor.inl"
#include "StaticMeshResources.h"
#include "RenderGraphResources.h"
#include "GlobalShader.h"
#include "RHIGPUReadback.h"
#include "RHI.h"
#include "RenderGraphUtils.h"

DECLARE_STATS_GROUP(TEXT("NoiseComputeShader"), STATGROUP_NoiseComputeShader, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("NoiseComputeShader Execute"), STAT_NoiseComputeShader_Execute, STATGROUP_NoiseComputeShader);

// This class carries our parameter declarations and acts as the bridge between cpp and HLSL.
class NOISEGENERATOR_API FNoiseComputeShader: public FGlobalShader
{
public:
	
	DECLARE_GLOBAL_SHADER(FNoiseComputeShader);
	SHADER_USE_PARAMETER_STRUCT(FNoiseComputeShader, FGlobalShader);
	
	
	class FNoiseComputeShader_Perm_TEST : SHADER_PERMUTATION_INT("TEST", 100);
	using FPermutationDomain = TShaderPermutationDomain<
		FNoiseComputeShader_Perm_TEST
	>;

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		/*
		* Here's where you define one or more of the input parameters for your shader.
		* Some examples:
		*/
		// SHADER_PARAMETER(uint32, MyUint32) // On the shader side: uint32 MyUint32;
		// SHADER_PARAMETER(FVector3f, MyVector) // On the shader side: float3 MyVector;

		// SHADER_PARAMETER_TEXTURE(Texture2D, MyTexture) // On the shader side: Texture2D<float4> MyTexture; (float4 should be whatever you expect each pixel in the texture to be, in this case float4(R,G,B,A) for 4 channels)
		// SHADER_PARAMETER_SAMPLER(SamplerState, MyTextureSampler) // On the shader side: SamplerState MySampler; // CPP side: TStaticSamplerState<ESamplerFilter::SF_Bilinear>::GetRHI();

		// SHADER_PARAMETER_ARRAY(float, MyFloatArray, [3]) // On the shader side: float MyFloatArray[3];

		// SHADER_PARAMETER_UAV(RWTexture2D<FVector4f>, MyTextureUAV) // On the shader side: RWTexture2D<float4> MyTextureUAV;
		// SHADER_PARAMETER_UAV(RWStructuredBuffer<FMyCustomStruct>, MyCustomStructs) // On the shader side: RWStructuredBuffer<FMyCustomStruct> MyCustomStructs;
		// SHADER_PARAMETER_UAV(RWBuffer<FMyCustomStruct>, MyCustomStructs) // On the shader side: RWBuffer<FMyCustomStruct> MyCustomStructs;

		// SHADER_PARAMETER_SRV(StructuredBuffer<FMyCustomStruct>, MyCustomStructs) // On the shader side: StructuredBuffer<FMyCustomStruct> MyCustomStructs;
		// SHADER_PARAMETER_SRV(Buffer<FMyCustomStruct>, MyCustomStructs) // On the shader side: Buffer<FMyCustomStruct> MyCustomStructs;
		// SHADER_PARAMETER_SRV(Texture2D<FVector4f>, MyReadOnlyTexture) // On the shader side: Texture2D<float4> MyReadOnlyTexture;

		// SHADER_PARAMETER_STRUCT_REF(FMyCustomStruct, MyCustomStruct)

		
		// SHADER_PARAMETER_RDG_BUFFER_SRV(Buffer<int>, Input)
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWBuffer<float>, Output)

		SHADER_PARAMETER(FVector3f, Position)
		SHADER_PARAMETER(FVector3f, Size)
		SHADER_PARAMETER(FVector3f, Offset)
		SHADER_PARAMETER(int, Mode)
		SHADER_PARAMETER(int, Octaves)
		SHADER_PARAMETER(float, Frequency)
		SHADER_PARAMETER(float, Lacunarity)
		SHADER_PARAMETER(float, Persistence)
		SHADER_PARAMETER(float, Scale)
		SHADER_PARAMETER(int, StepSize)
		SHADER_PARAMETER(int, Filter)
		SHADER_PARAMETER(int, Type)
		SHADER_PARAMETER(int, DensityFunction)
		SHADER_PARAMETER(int, NormalizeMode)
		SHADER_PARAMETER(float, DomainWarp)

	END_SHADER_PARAMETER_STRUCT()

public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		const FPermutationDomain PermutationVector(Parameters.PermutationId);
		
		return true;
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);

		const FPermutationDomain PermutationVector(Parameters.PermutationId);

		/*
		* Here you define constants that can be used statically in the shader code.
		* Example:
		*/
		// OutEnvironment.SetDefine(TEXT("MY_CUSTOM_CONST"), TEXT("1"));

		/*
		* These defines are used in the thread count section of our shader
		*/
		OutEnvironment.SetDefine(TEXT("THREADS_X"), NUM_THREADS_NoiseComputeShader_X);
		OutEnvironment.SetDefine(TEXT("THREADS_Y"), NUM_THREADS_NoiseComputeShader_Y);
		OutEnvironment.SetDefine(TEXT("THREADS_Z"), NUM_THREADS_NoiseComputeShader_Z);

		// This shader must support typed UAV load and we are testing if it is supported at runtime using RHIIsTypedUAVLoadSupported
		//OutEnvironment.CompilerFlags.Add(CFLAG_AllowTypedUAVLoads);

		// FForwardLightingParameters::ModifyCompilationEnvironment(Parameters.Platform, OutEnvironment);
	}
private:
};

// This will tell the engine to create the shader and where the shader entry point is.
//                            ShaderType                            ShaderPath                     Shader function name    Type
IMPLEMENT_GLOBAL_SHADER(FNoiseComputeShader, "/NoiseShaders/NoiseComputeShader.usf", "NoiseComputeShader", SF_Compute);

FNoiseComputeShaderDispatchParams FNoiseComputeShaderInterface::BuildParams(FVector3f Position, FVector3f Size, int StepSize, FNoiseSettings NoiseSettings, TEnumAsByte<NoiseMode> NoiseMode,TEnumAsByte<NoiseDensityFunction> DensityFunction)
{
	FNoiseComputeShaderDispatchParams Params(Size.X, Size.Y, Size.Z);
		
	Params.Position = Position;
	Params.Size = Size;
	Params.Offset = (FVector3f)NoiseSettings.offset;
	Params.Mode = NoiseMode.GetValue();
	Params.Octaves = NoiseSettings.octaves;
	Params.Frequency = NoiseSettings.frequency;
	Params.Lacunarity = NoiseSettings.lacunarity;
	Params.Persistence = NoiseSettings.persistence;
	Params.Scale = NoiseSettings.scale;
	Params.StepSize = StepSize;
	Params.Filter = NoiseSettings.filter.GetValue();
	Params.DensityFunction = DensityFunction.GetValue();
	Params.Type = NoiseSettings.type.GetValue();
	Params.NormalizeMode = NoiseSettings.normalizeMode.GetValue();
	Params.DomainWarp = NoiseSettings.domainWarping ? NoiseSettings.domainWarpingScale : 0;

	return Params;
}

void FNoiseComputeShaderInterface::DispatchRenderThread(FRHICommandListImmediate& RHICmdList, FNoiseComputeShaderDispatchParams Params, TFunction<void(TArray<float> OutputVals)> AsyncCallback) {
	FRDGBuilder GraphBuilder(RHICmdList);

	{
		SCOPE_CYCLE_COUNTER(STAT_NoiseComputeShader_Execute);
		DECLARE_GPU_STAT(NoiseComputeShader)
		RDG_EVENT_SCOPE(GraphBuilder, "NoiseComputeShader");
		RDG_GPU_STAT_SCOPE(GraphBuilder, NoiseComputeShader);
		
		typename FNoiseComputeShader::FPermutationDomain PermutationVector;
		PermutationVector.Set<FNoiseComputeShader::FNoiseComputeShader_Perm_TEST>(1);
		TShaderMapRef<FNoiseComputeShader> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel), PermutationVector);
		
		bool bIsShaderValid = ComputeShader.IsValid();

		if (bIsShaderValid) {
			FNoiseComputeShader::FParameters* PassParameters = GraphBuilder.AllocParameters<FNoiseComputeShader::FParameters>();

			PassParameters->Position = Params.Position;
			PassParameters->Size = Params.Size;
			PassParameters->Offset = Params.Offset;
			PassParameters->Mode = Params.Mode;
			PassParameters->Octaves = Params.Octaves;
			PassParameters->Frequency = Params.Frequency;
			PassParameters->Lacunarity = Params.Lacunarity;
			PassParameters->Persistence = Params.Persistence;
			PassParameters->Scale = Params.Scale;
			PassParameters->StepSize = Params.StepSize;
			PassParameters->Filter = Params.Filter;
			PassParameters->Type = Params.Type;
			PassParameters->DensityFunction = Params.DensityFunction;
			PassParameters->NormalizeMode = Params.NormalizeMode;
			PassParameters->DomainWarp = Params.DomainWarp;
			
			int NumOutputs = Params.Size.X*Params.Size.Y*Params.Size.Z;
			FRDGBufferRef OutputBuffer = GraphBuilder.CreateBuffer(FRDGBufferDesc::CreateBufferDesc(sizeof(float), NumOutputs),TEXT("OutputBuffer"));
			PassParameters->Output = GraphBuilder.CreateUAV(FRDGBufferUAVDesc(OutputBuffer, PF_R32_FLOAT));
			
			auto GroupCount = FComputeShaderUtils::GetGroupCount(FIntVector(Params.X, Params.Y,Params.Z), FComputeShaderUtils::kGolden2DGroupSize);
			GraphBuilder.AddPass(RDG_EVENT_NAME("ExecuteNoiseComputeShader"),PassParameters,ERDGPassFlags::AsyncCompute,[&PassParameters, ComputeShader, GroupCount](FRHIComputeCommandList& RHICmdList){
				FComputeShaderUtils::Dispatch(RHICmdList, ComputeShader, *PassParameters, GroupCount);
			});

			FRHIGPUBufferReadback* GPUBufferReadback = new FRHIGPUBufferReadback(TEXT("ExecuteNoiseComputeShaderOutput"));
			AddEnqueueCopyPass(GraphBuilder, GPUBufferReadback, OutputBuffer, 0u);

			auto RunnerFunc = [GPUBufferReadback, NumOutputs, AsyncCallback](auto&& RunnerFunc) -> void {
				if (GPUBufferReadback->IsReady()) {
					float* Buffer = (float*)GPUBufferReadback->Lock(NumOutputs);
					TArray<float> OutputVals;
					OutputVals.SetNum(NumOutputs);
					FMemory::Memcpy(OutputVals.GetData(), Buffer, NumOutputs * sizeof(float));
					
					GPUBufferReadback->Unlock();

					AsyncTask(ENamedThreads::GameThread, [AsyncCallback, OutputVals]() {
						AsyncCallback(OutputVals);
					});

					delete GPUBufferReadback;
				} else {
					AsyncTask(ENamedThreads::ActualRenderingThread, [RunnerFunc]() {
						RunnerFunc(RunnerFunc);
					});
				}
			};

			AsyncTask(ENamedThreads::ActualRenderingThread, [RunnerFunc]() {
				RunnerFunc(RunnerFunc);
			});
			
		} else {
			#if WITH_EDITOR
				GEngine->AddOnScreenDebugMessage((uint64)42145125184, 6.f, FColor::Red, FString(TEXT("The compute shader has a problem.")));
			#endif
			// We exit here as we don't want to crash the game if the shader is not found or has an error.
		}
	}

	GraphBuilder.Execute();
}

void FNoiseComputeShaderInterface::DispatchMyComputeShader(FRDGBuilder& GraphBuilder, FRDGBufferRef OutputBuffer, FNoiseComputeShaderDispatchParams Params,int index)
{
	FNoiseComputeShader::FParameters* PassParameters = GraphBuilder.AllocParameters<FNoiseComputeShader::FParameters>();
	PassParameters->Position = Params.Position;
	PassParameters->Size = Params.Size;
	PassParameters->Offset = Params.Offset;
	PassParameters->Mode = Params.Mode;
	PassParameters->Octaves = Params.Octaves;
	PassParameters->Frequency = Params.Frequency;
	PassParameters->Lacunarity = Params.Lacunarity;
	PassParameters->Persistence = Params.Persistence;
	PassParameters->Scale = Params.Scale;
	PassParameters->StepSize = Params.StepSize;
	PassParameters->Filter = Params.Filter;
	PassParameters->Type = Params.Type;
	PassParameters->DensityFunction = Params.DensityFunction;
	PassParameters->NormalizeMode = Params.NormalizeMode;
	PassParameters->DomainWarp = Params.DomainWarp;
	PassParameters->Output = GraphBuilder.CreateUAV(FRDGBufferUAVDesc(OutputBuffer, PF_R32_FLOAT));
	
	typename FNoiseComputeShader::FPermutationDomain PermutationVector;
	PermutationVector.Set<FNoiseComputeShader::FNoiseComputeShader_Perm_TEST>(1+index);
	TShaderMapRef<FNoiseComputeShader> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel),PermutationVector);
	FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("MyComputeShaderPass"), ComputeShader, PassParameters, FIntVector(Params.X, Params.Y, Params.Z));
}

void FNoiseComputeShaderInterface::ExecuteMultipleDispatchesAndNotify(FRHICommandListImmediate& RHICmdList, TArray<FNoiseComputeShaderDispatchParams> Params, TFunction<void(TArray<TArray<float>> OutputVals)> AsyncCallback)
{
    FRDGBuilder GraphBuilder(RHICmdList);
    TArray<FRDGBufferRef> OutputBuffers;
    TArray<FRHIGPUBufferReadback*> GPUBufferReadbacks;

    // Create buffers and dispatch compute shaders
    for (int32 i = 0; i < Params.Num(); ++i) {
        int NumOutputs = Params[i].Size.X * Params[i].Size.Y * Params[i].Size.Z;
        FRDGBufferRef OutputBuffer = GraphBuilder.CreateBuffer(FRDGBufferDesc::CreateBufferDesc(sizeof(float), NumOutputs), TEXT("OutputBuffer"));
        OutputBuffers.Add(OutputBuffer);

        DispatchMyComputeShader(GraphBuilder, OutputBuffer, Params[i], i);

        FRHIGPUBufferReadback* GPUBufferReadback = new FRHIGPUBufferReadback(TEXT("ExecuteNoiseComputeShaderOutput"));
        GPUBufferReadbacks.Add(GPUBufferReadback);
        AddEnqueueCopyPass(GraphBuilder, GPUBufferReadback, OutputBuffer, 0u);
    }

    // Execute the graph
    GraphBuilder.Execute();

    // Flush the GPU commands and submit them for execution
    RHICmdList.SubmitCommandsAndFlushGPU();

    // After graph execution, perform GPU buffer readback on the rendering thread
    AsyncTask(ENamedThreads::ActualRenderingThread, [GPUBufferReadbacks, Params, AsyncCallback]() {
        TArray<TArray<float>> OutputData;
        OutputData.SetNum(Params.Num());

        for (int32 i = 0; i < GPUBufferReadbacks.Num(); ++i) {
            FRHIGPUBufferReadback* GPUBufferReadback = GPUBufferReadbacks[i];
            if (GPUBufferReadback->IsReady()) {
                int NumOutputs = Params[i].Size.X * Params[i].Size.Y * Params[i].Size.Z;
                float* Buffer = (float*)GPUBufferReadback->Lock(NumOutputs);
                TArray<float> OutputVals;
                OutputVals.SetNum(NumOutputs);
                FMemory::Memcpy(OutputVals.GetData(), Buffer, NumOutputs * sizeof(float));
                GPUBufferReadback->Unlock();

                OutputData[i] = OutputVals;
            } else {
                UE_LOG(NoiseGenerator, Warning, TEXT("GPU Buffer Readback is not ready for index %d."), i);
            }

            delete GPUBufferReadback;
        }

        // Call the callback on the game thread with the results
        AsyncTask(ENamedThreads::GameThread, [AsyncCallback, OutputData]() {
            AsyncCallback(OutputData);
        });
    });
}

