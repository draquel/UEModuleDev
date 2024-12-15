#include "NoiseTextureComputeShader.h"
#include "NoiseGenerator/Public/NoiseTextureComputeShader/NoiseTextureComputeShader.h"

#include "PixelShaderUtils.h"
#include "MeshPassProcessor.inl"
#include "StaticMeshResources.h"
#include "RenderGraphResources.h"
#include "GlobalShader.h"
#include "NoiseSettings.h"
#include "RHIGPUReadback.h"
#include "SceneTexturesConfig.h"

DECLARE_STATS_GROUP(TEXT("NoiseTextureComputeShader"), STATGROUP_NoiseTextureComputeShader, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("NoiseTextureComputeShader Execute"), STAT_NoiseTextureComputeShader_Execute, STATGROUP_NoiseTextureComputeShader);

// This class carries our parameter declarations and acts as the bridge between cpp and HLSL.
class NOISEGENERATOR_API FNoiseTextureComputeShader: public FGlobalShader
{
public:
	
	DECLARE_GLOBAL_SHADER(FNoiseTextureComputeShader);
	SHADER_USE_PARAMETER_STRUCT(FNoiseTextureComputeShader, FGlobalShader);
	
	
	class FNoiseTextureComputeShader_Perm_TEST : SHADER_PERMUTATION_INT("TEST", 1);
	using FPermutationDomain = TShaderPermutationDomain<
		FNoiseTextureComputeShader_Perm_TEST
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

		
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, RenderTarget)

		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FShaderNoiseSettings>, NoiseSettings)

		// SHADER_PARAMETER(int, Mode)
		// SHADER_PARAMETER(int, DensityFunction)
	
		SHADER_PARAMETER(FVector3f, Position)
		SHADER_PARAMETER(FVector3f, Size)
		SHADER_PARAMETER(int, StepSize)
		SHADER_PARAMETER(int, SettingsSize)
	
		// SHADER_PARAMETER(FVector3f, Offset)
		// SHADER_PARAMETER(int, Octaves)
		// SHADER_PARAMETER(float, Frequency)
		// SHADER_PARAMETER(float, Lacunarity)
		// SHADER_PARAMETER(float, Persistence)
		// SHADER_PARAMETER(float, Scale)
		// SHADER_PARAMETER(int, Filter)
		// SHADER_PARAMETER(int, Type)
		// SHADER_PARAMETER(int, NormalizeMode)
		// SHADER_PARAMETER(float, DomainWarp)	

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
		OutEnvironment.SetDefine(TEXT("THREADS_X"), NUM_THREADS_NoiseTextureComputeShader_X);
		OutEnvironment.SetDefine(TEXT("THREADS_Y"), NUM_THREADS_NoiseTextureComputeShader_Y);
		OutEnvironment.SetDefine(TEXT("THREADS_Z"), NUM_THREADS_NoiseTextureComputeShader_Z);

		// This shader must support typed UAV load and we are testing if it is supported at runtime using RHIIsTypedUAVLoadSupported
		//OutEnvironment.CompilerFlags.Add(CFLAG_AllowTypedUAVLoads);

		// FForwardLightingParameters::ModifyCompilationEnvironment(Parameters.Platform, OutEnvironment);
	}
private:
};

// This will tell the engine to create the shader and where the shader entry point is.
//                            ShaderType                            ShaderPath                     Shader function name    Type
IMPLEMENT_GLOBAL_SHADER(FNoiseTextureComputeShader, "/NoiseShaders/NoiseTextureComputeShader.usf", "NoiseTextureComputeShader", SF_Compute);

FNoiseTextureComputeShaderDispatchParams FNoiseTextureComputeShaderInterface::BuildParams(UTextureRenderTarget2D* RenderTarget, FVector3f Position, FVector3f Size, int StepSize, TArray<FNoiseSettings> NoiseSettings)
{
	TArray<FShaderNoiseSettings> ShaderNoiseSettings;
	for (int i = 0; i < NoiseSettings.Num(); i++) ShaderNoiseSettings.Add(FShaderNoiseSettings(&NoiseSettings[i]));
	
	FNoiseTextureComputeShaderDispatchParams Params(Size.X, Size.Y, Size.Z);
		
	Params.RenderTarget = RenderTarget->GameThread_GetRenderTargetResource();
	Params.Position = Position;
	Params.Size = Size;
	Params.StepSize = StepSize;
	Params.NoiseSettings = ShaderNoiseSettings;
	Params.SettingsSize = ShaderNoiseSettings.Num();
	
	return Params;
}

void FNoiseTextureComputeShaderInterface::DispatchRenderThread(FRHICommandListImmediate& RHICmdList, FNoiseTextureComputeShaderDispatchParams Params)
{
	FRDGBuilder GraphBuilder(RHICmdList);

	{
		SCOPE_CYCLE_COUNTER(STAT_NoiseTextureComputeShader_Execute);
		DECLARE_GPU_STAT(NoiseTextureComputeShader)
		RDG_EVENT_SCOPE(GraphBuilder, "NoiseTextureComputeShader");
		RDG_GPU_STAT_SCOPE(GraphBuilder, NoiseTextureComputeShader);
		
		typename FNoiseTextureComputeShader::FPermutationDomain PermutationVector;
		
		// Add any static permutation options here
		// PermutationVector.Set<FNoiseTextureComputeShader::FMyPermutationName>(12345);

		TShaderMapRef<FNoiseTextureComputeShader> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel), PermutationVector);
		

		bool bIsShaderValid = ComputeShader.IsValid();

		if (bIsShaderValid) {
			FNoiseTextureComputeShader::FParameters* PassParameters = GraphBuilder.AllocParameters<FNoiseTextureComputeShader::FParameters>();

			FRDGTextureDesc Desc(FRDGTextureDesc::Create2D(Params.RenderTarget->GetSizeXY(), PF_R16F, FClearValueBinding::White, TexCreate_RenderTargetable | TexCreate_ShaderResource | TexCreate_UAV));
			FRDGTextureRef TmpTexture = GraphBuilder.CreateTexture(Desc, TEXT("NoiseTextureComputeShader_TempTexture"));
			FRDGTextureRef TargetTexture = RegisterExternalTexture(GraphBuilder, Params.RenderTarget->GetRenderTargetTexture(), TEXT("NoiseTextureComputeShader_RT"));
			PassParameters->RenderTarget = GraphBuilder.CreateUAV(TmpTexture);
			PassParameters->Position = Params.Position;
			PassParameters->Size = Params.Size;
			PassParameters->StepSize = Params.StepSize;
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
			
			auto GroupCount = FComputeShaderUtils::GetGroupCount(FIntVector(Params.X, Params.Y, Params.Z), FComputeShaderUtils::kGolden2DGroupSize);
			GraphBuilder.AddPass(
				RDG_EVENT_NAME("ExecuteNoiseTextureComputeShader"),
				PassParameters,
				ERDGPassFlags::AsyncCompute,
				[&PassParameters, ComputeShader, GroupCount](FRHIComputeCommandList& RHICmdList)
			{
				FComputeShaderUtils::Dispatch(RHICmdList, ComputeShader, *PassParameters, GroupCount);
			});

			
			// The copy will fail if we don't have matching formats, let's check and make sure we do.
			if (TargetTexture->Desc.Format == PF_R16F) {
				AddCopyTexturePass(GraphBuilder, TmpTexture, TargetTexture, FRHICopyTextureInfo());
			} else {
#if WITH_EDITOR
				GEngine->AddOnScreenDebugMessage((uint64)42145125184, 6.f, FColor::Red, FString(TEXT("The provided render target has an incompatible format (Please change the RT format to: RGBA8).")));
#endif
			}
			
		} else {
#if WITH_EDITOR
			GEngine->AddOnScreenDebugMessage((uint64)42145125184, 6.f, FColor::Red, FString(TEXT("The compute shader has a problem.")));
#endif

			// We exit here as we don't want to crash the game if the shader is not found or has an error.
			
		}
	}

	GraphBuilder.Execute();
}

