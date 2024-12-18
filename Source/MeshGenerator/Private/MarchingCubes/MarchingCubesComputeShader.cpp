#include "MarchingCubesComputeShader.h"
#include "MeshGenerator/Public/MarchingCubes/MarchingCubesComputeShader.h"
#include "PixelShaderUtils.h"
#include "MeshPassProcessor.inl"
#include "StaticMeshResources.h"
#include "RenderGraphResources.h"
#include "GlobalShader.h"
#include "RHIGPUReadback.h"
#include "RHI.h"
#include "RenderGraphUtils.h"

DECLARE_STATS_GROUP(TEXT("MarchingCubesComputeShader"), STATGROUP_MarchingCubesComputeShader, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("MarchingCubesComputeShader Execute"), STAT_MarchingCubesComputeShader_Execute, STATGROUP_MarchingCubesComputeShader);

// This class carries our parameter declarations and acts as the bridge between cpp and HLSL.
class MESHGENERATOR_API FMarchingCubesComputeShader: public FGlobalShader
{
public:
	
	DECLARE_GLOBAL_SHADER(FMarchingCubesComputeShader);
	SHADER_USE_PARAMETER_STRUCT(FMarchingCubesComputeShader, FGlobalShader);
	
	
	class FMarchingCubesComputeShader_Perm_TEST : SHADER_PERMUTATION_INT("TEST", 100);
	using FPermutationDomain = TShaderPermutationDomain<
		FMarchingCubesComputeShader_Perm_TEST
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

		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FShaderNoiseSettings>, NoiseSettings)

	
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<FVector4f>, points)
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<FTriangle>, triangles)

	
		SHADER_PARAMETER(int, SettingsSize)

		SHADER_PARAMETER(FVector3f, Position)
		SHADER_PARAMETER(FVector3f, Size)
		SHADER_PARAMETER(int, StepSize)
		SHADER_PARAMETER(int, Mode)
		SHADER_PARAMETER(int, DensityFunction)
	
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
		OutEnvironment.SetDefine(TEXT("THREADS_X"), NUM_THREADS_MarchingCubesComputeShader_X);
		OutEnvironment.SetDefine(TEXT("THREADS_Y"), NUM_THREADS_MarchingCubesComputeShader_Y);
		OutEnvironment.SetDefine(TEXT("THREADS_Z"), NUM_THREADS_MarchingCubesComputeShader_Z);

		// This shader must support typed UAV load and we are testing if it is supported at runtime using RHIIsTypedUAVLoadSupported
		//OutEnvironment.CompilerFlags.Add(CFLAG_AllowTypedUAVLoads);

		// FForwardLightingParameters::ModifyCompilationEnvironment(Parameters.Platform, OutEnvironment);
	}
private:
};

// This will tell the engine to create the shader and where the shader entry point is.
//                            ShaderType                            ShaderPath                     Shader function name    Type
IMPLEMENT_GLOBAL_SHADER(FMarchingCubesComputeShader, "/MeshShaders/MarchingCubes.usf", "March", SF_Compute);

FMarchingCubesComputeShaderDispatchParams FMarchingCubesComputeShaderInterface::BuildParams(FVector3f Position, FVector3f Size, int StepSize, TArray<FNoiseSettings> NoiseSettings, TEnumAsByte<NoiseMode> NoiseMode,TEnumAsByte<NoiseDensityFunction> DensityFunction)
{
	TArray<FShaderNoiseSettings> ShaderNoiseSettings;
	for (int i = 0; i < NoiseSettings.Num(); i++) ShaderNoiseSettings.Add(FShaderNoiseSettings(&NoiseSettings[i]));
	
	FMarchingCubesComputeShaderDispatchParams Params(Size.X, Size.Y, Size.Z);
		
	Params.Position = Position;
	Params.Size = Size;
	Params.StepSize = StepSize;
	Params.Mode = NoiseMode.GetValue();
	Params.DensityFunction = DensityFunction.GetValue();
	Params.NoiseSettings = ShaderNoiseSettings;
	Params.SettingsSize = ShaderNoiseSettings.Num();
	
	return Params;
}

void FMarchingCubesComputeShaderInterface::DispatchRenderThread(FRHICommandListImmediate& RHICmdList, FMarchingCubesComputeShaderDispatchParams Params, TFunction<void(TArray<FVector4f>& Verts, TArray<FMeshTriangle>& Tris)> AsyncCallback) {
	FRDGBuilder GraphBuilder(RHICmdList);

	{
		SCOPE_CYCLE_COUNTER(STAT_MarchingCubesComputeShader_Execute);
		DECLARE_GPU_STAT(MarchingCubesComputeShader)
		RDG_EVENT_SCOPE(GraphBuilder, "MarchingCubesComputeShader");
		RDG_GPU_STAT_SCOPE(GraphBuilder, MarchingCubesComputeShader);
		
		typename FMarchingCubesComputeShader::FPermutationDomain PermutationVector;
		PermutationVector.Set<FMarchingCubesComputeShader::FMarchingCubesComputeShader_Perm_TEST>(1);
		TShaderMapRef<FMarchingCubesComputeShader> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel), PermutationVector);
		
		bool bIsShaderValid = ComputeShader.IsValid();

		if (bIsShaderValid) {
			FMarchingCubesComputeShader::FParameters* PassParameters = GraphBuilder.AllocParameters<FMarchingCubesComputeShader::FParameters>();

			PassParameters->Position = Params.Position;
			PassParameters->Size = Params.Size;
			PassParameters->StepSize = Params.StepSize;
			PassParameters->Mode = Params.Mode;
			PassParameters->DensityFunction = Params.DensityFunction;
			PassParameters->SettingsSize = Params.SettingsSize;

			FRDGBufferRef NoiseSettingsBuffer = CreateStructuredBuffer(
				GraphBuilder,
				TEXT("NoiseSettingsBuffer"),
				sizeof(FShaderNoiseSettings),
				Params.SettingsSize,
				Params.NoiseSettings.GetData(),
				Params.SettingsSize * sizeof(FShaderNoiseSettings)
			);
			FRDGBufferSRVRef NoiseSettingsBufferSRV = GraphBuilder.CreateSRV(NoiseSettingsBuffer);
			PassParameters->NoiseSettings = NoiseSettingsBufferSRV;
			
			int NumOutputs = Params.Size.X*Params.Size.Y*Params.Size.Z;
			FRDGBufferRef OutputBuffer = GraphBuilder.CreateBuffer(FRDGBufferDesc::CreateBufferDesc(sizeof(float), NumOutputs),TEXT("OutputBuffer"));
			PassParameters->Output = GraphBuilder.CreateUAV(FRDGBufferUAVDesc(OutputBuffer, PF_R32_FLOAT));
			
			auto GroupCount = FComputeShaderUtils::GetGroupCount(FIntVector(Params.X, Params.Y,Params.Z), FComputeShaderUtils::kGolden2DGroupSize);
			GraphBuilder.AddPass(RDG_EVENT_NAME("ExecuteMarchingCubesComputeShader"),PassParameters,ERDGPassFlags::AsyncCompute,[&PassParameters, ComputeShader, GroupCount](FRHIComputeCommandList& RHICmdList){
				FComputeShaderUtils::Dispatch(RHICmdList, ComputeShader, *PassParameters, GroupCount);
			});

			FRHIGPUBufferReadback* GPUBufferReadback = new FRHIGPUBufferReadback(TEXT("ExecuteMarchingCubesComputeShaderOutput"));
			AddEnqueueCopyPass(GraphBuilder, GPUBufferReadback, OutputBuffer, 0u);

			auto RunnerFunc = [GPUBufferReadback, NumOutputs, AsyncCallback](auto&& RunnerFunc) -> void {
				if (GPUBufferReadback->IsReady()) {
					float* Buffer = (float*)GPUBufferReadback->Lock(NumOutputs);
					TArray<float> OutputVals;
					OutputVals.SetNum(NumOutputs);
					FMemory::Memcpy(OutputVals.GetData(), Buffer, NumOutputs * sizeof(float));
					
					GPUBufferReadback->Unlock();

					AsyncTask(ENamedThreads::GameThread, [AsyncCallback, OutputVals]() {
						// AsyncCallback(OutputVals);
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