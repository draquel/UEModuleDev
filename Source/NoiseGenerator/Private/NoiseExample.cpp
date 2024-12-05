#include "NoiseExample.h"
#include "Noise.h"
#include "NoiseGenerator.h"
#include "NoiseGenerator/Public/NoiseComputeShader/NoiseComputeShader.h"

ANoiseExample::ANoiseExample()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	TMesh = CreateDefaultSubobject<UStaticMeshComponent>("Texture Mesh");
	TMesh->SetupAttachment(RootComponent);
}

void ANoiseExample::Regenerate()
{
	if (UseLayeredNoise) {
		GenerateLayered();
	} else { 
		Generate();
	}
}

void ANoiseExample::RandomSeed()
{
	if (UseLayeredNoise){
		for(int i = 0; i < LayeredNoiseSettings.Num(); i++)	{
			LayeredNoiseSettings[i].LayerSettings.seed = FMath::RandRange(-100000000,100000000);	
		}	
	}else{
		NoiseSettings.seed = FMath::RandRange(-100000000,100000000);
	}
	
	Regenerate();
}

void ANoiseExample::BeginPlay()
{
	Super::BeginPlay();
	Regenerate();
}

void ANoiseExample::Generate()
{
	int cycles = TextureSize.X * TextureSize.Y * NoiseSettings.octaves;
	double start = FPlatformTime::Seconds();

	if(NoiseSettings.source == CPU) {
		SetTexture(UNoise::GenerateTexture(Center,TextureSize,&NoiseSettings,ColorCurve));
		double end = FPlatformTime::Seconds();
		UE_LOG(NoiseGenerator,Log,TEXT("NoiseExample::Generate() ==> %s-%s, Cycles: %d, RunTime: %fs"),*UEnum::GetValueAsString(NoiseSettings.source),*UEnum::GetValueAsString(NoiseSettings.type),cycles,end-start);
	}else{
		FNoiseComputeShaderDispatchParams Params = FNoiseComputeShaderInterface::BuildParams((FVector3f)Center, FVector3f(TextureSize.X,TextureSize.Y,1.0f), NoiseSettings);
		FNoiseComputeShaderInterface::Dispatch(Params,[this,cycles,start](TArray<float> OutputVals){
			FNoiseMap2d NoiseMap = FNoiseMap2d(Center,FIntVector2(TextureSize.X,TextureSize.Y),OutputVals);
			if (NoiseSettings.normalizeMode == Local || NoiseSettings.normalizeMode == LocalPositive){
				UNoise::Normalize(NoiseMap,NoiseSettings.normalizeMode,NoiseSettings.type);
			}
			SetTexture(UNoise::GenerateTexture(&NoiseMap,ColorCurve));
			double end = FPlatformTime::Seconds();
			UE_LOG(NoiseGenerator,Log,TEXT("NoiseExample::Generate() ==> %s-%s, Cycles: %d, RunTime: %fs"),*UEnum::GetValueAsString(NoiseSettings.source),*UEnum::GetValueAsString(NoiseSettings.type),cycles,end-start);
		});
	}
}

void ANoiseExample::GenerateLayered()
{
	int cycles = 0;
	for (auto Layer : LayeredNoiseSettings){ cycles += (TextureSize.X * TextureSize.Y) * Layer.LayerSettings.octaves; }	
	double start = FPlatformTime::Seconds();
	
	TArray<FNoiseComputeShaderDispatchParams> Params;	
	for (int i = 0; i < LayeredNoiseSettings.Num(); i++){
		if (LayeredNoiseSettings[i].LayerSettings.source == GPU){
			Params.Add(FNoiseComputeShaderInterface::BuildParams((FVector3f)Center, FVector3f(TextureSize.X,TextureSize.Y,1.0f),LayeredNoiseSettings[i].LayerSettings));
		}
	}

	FNoiseComputeShaderInterface::LayeredDispatch(Params,[this,cycles,start](TArray<TArray<float>> OutputVals)	{
		int available = OutputVals.Num(); // available buffers
		int used = 0; // buffers checked / converted
		TArray<FNoiseLayerData> ResultData;
		for (int i = 0; i < LayeredNoiseSettings.Num(); i++) {
			if (LayeredNoiseSettings[i].LayerSettings.source == CPU) {
				ResultData.Add(FNoiseLayerData(UNoise::GenerateMap2D(Center,TextureSize,&LayeredNoiseSettings[i].LayerSettings),LayeredNoiseSettings[i].Gain,LayeredNoiseSettings[i].LayerCurve));
			}else {
				if (used >= available) { continue; } //Checked all buffers
			 	if (OutputVals[used].Num() == 0) { used++; continue; } //Buffer has no data	
				ResultData.Add(FNoiseLayerData(FNoiseMap2d(Center,TextureSize,OutputVals[used]),LayeredNoiseSettings[i].Gain,LayeredNoiseSettings[i].LayerCurve));
				if (LayeredNoiseSettings[i].LayerSettings.normalizeMode == Local || LayeredNoiseSettings[i].LayerSettings.normalizeMode == LocalPositive){
					UNoise::Normalize(ResultData[i].noiseMap,LayeredNoiseSettings[i].LayerSettings.normalizeMode, LayeredNoiseSettings[i].LayerSettings.type);
				}
				used++;
			}
		}

		FNoiseMap2d NoiseMap = FNoiseMap2d(Center,TextureSize);
		UNoise::GenerateMap2D(NoiseMap,&ResultData);
		SetTexture(UNoise::GenerateTexture(&NoiseMap,ColorCurve));
		double end = FPlatformTime::Seconds();
		UE_LOG(NoiseGenerator,Log,TEXT("NoiseExample::GenerateLayered() ==> %s [%d], Cycles: %d, RunTime: %f"),TEXT("Layered"),ResultData.Num(),cycles,end-start);
	});
}

void ANoiseExample::SetTexture(UTexture2D* Texture)
{
	DynamicMaterial = UMaterialInstanceDynamic::Create(DefaultMaterial,this);
	DynamicMaterial->SetTextureParameterValue(FName("DynamicTexture"), Texture);
	TMesh->SetMaterial(0, DynamicMaterial);	
}

void ANoiseExample::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANoiseExample::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	Regenerate();
}
