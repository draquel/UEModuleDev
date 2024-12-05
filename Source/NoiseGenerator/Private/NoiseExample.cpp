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
	double start = FPlatformTime::Seconds();
	int cycles = 0;
	if (UseLayeredNoise){ for (auto Layer : LayeredNoiseSettings){ cycles += (TextureSize.X * TextureSize.Y) * Layer.LayerSettings.octaves; }	}
	else{ cycles += TextureSize.X * TextureSize.Y * NoiseSettings.octaves; }
	FString type = TEXT("CPU Noise");
	
	if (UseLayeredNoise) {
		type = TEXT("Layered Noise");

		TArray<FNoiseLayerData> LayerData;
		for (int i = 0; i < LayeredNoiseSettings.Num(); i++){
			LayerData.Add(FNoiseLayerData());
			LayerData[i].curve = LayeredNoiseSettings[i].LayerCurve;
			LayerData[i].gain = LayeredNoiseSettings[i].Gain;
		}

		TArray<FNoiseComputeShaderDispatchParams> Params;
		for (int i = 0; i < LayerData.Num(); i++) {
			if (LayeredNoiseSettings[i].LayerSettings.type == Perlin || LayeredNoiseSettings[i].LayerSettings.type == Simplex) {
				LayerData[i].noiseMap = UNoise::GenerateMap2D(Center,TextureSize,&LayeredNoiseSettings[i].LayerSettings);
			}else {
				Params.Add(FNoiseComputeShaderInterface::BuildParams((FVector3f)Center, FVector3f(TextureSize.X,TextureSize.Y,1.0f),LayeredNoiseSettings[i].LayerSettings));
			}
		}

		FNoiseComputeShaderInterface::LayeredDispatch(Params,[this,LayerData,type,cycles,start](TArray<TArray<float>> OutputVals)	{
			// UE_LOG(LogTemp, Display, TEXT("ANoiseExample::Regenerate - Layered Noise GPU Callback (%d/%d)"),LayerData.Num(),OutputVals.Num());
			int available = OutputVals.Num();
			int used = 0;
			int gain = 0;
			TArray<FNoiseLayerData> ResultData;
			for (int i = 0; i < LayerData.Num(); i++) {
				gain += LayerData[i].gain;
				
				if (LayeredNoiseSettings[i].LayerSettings.type == Perlin || LayeredNoiseSettings[i].LayerSettings.type == Simplex) {
					// UE_LOG(LogTemp, Display, TEXT("ANoiseExample::Regenerate - Layered Noise GPU Callback - Adding CPU MAP %d (%d)"),i,LayerData[i].noiseMap.Map.Num());
					ResultData.Add(LayerData[i]);
					continue;
				}
				
				if (used >= available) { break; }
				if (OutputVals[used].Num() == 0) { used++; continue; }

				// UE_LOG(LogTemp, Display, TEXT("ANoiseExample::Regenerate - Layered Noise GPU Callback - Adding GPU MAP %d (%d)"),i,OutputVals[used].Num());
				ResultData.Add(FNoiseLayerData(FNoiseMap2d(Center,TextureSize,OutputVals[used]),LayerData[i].gain,LayerData[i].curve));
				if (LayeredNoiseSettings[i].LayerSettings.normalizeMode == Local || LayeredNoiseSettings[i].LayerSettings.normalizeMode == LocalPositive){
					ResultData[i].noiseMap = UNoise::Normalize(ResultData[i].noiseMap,LayeredNoiseSettings[i].LayerSettings.normalizeMode, LayeredNoiseSettings[i].LayerSettings.type);
				}
				used++;
			}

			FNoiseMap2d NoiseMap = FNoiseMap2d(Center,TextureSize);
			UNoise::GenerateMap2D(NoiseMap,&ResultData);
			SetTexture(UNoise::GenerateTexture(NoiseMap,ColorCurve));
			double end = FPlatformTime::Seconds();
			UE_LOG(NoiseGenerator,Log,TEXT("NoiseExample::Regenerate() ==> Type: %s, Cycles: %d, RunTime: %fs"),*type,cycles,end-start);	
		});
	} else {
		if (NoiseSettings.type == NoiseType::GPU_Perlin || NoiseSettings.type == NoiseType::GPU_Simplex){
			type = TEXT("GPU Noise");
			FNoiseComputeShaderDispatchParams Params = FNoiseComputeShaderInterface::BuildParams((FVector3f)Center, FVector3f(TextureSize.X,TextureSize.Y,1.0f), NoiseSettings);
		
			FNoiseComputeShaderInterface::Dispatch(Params,[this,type,cycles,start](TArray<float> OutputVals){
				FNoiseMap2d NoiseMap = FNoiseMap2d(Center,FIntVector2(TextureSize.X,TextureSize.Y),OutputVals);
				if (NoiseSettings.normalizeMode == Local || NoiseSettings.normalizeMode == LocalPositive){
					NoiseMap = UNoise::Normalize(NoiseMap,NoiseSettings.normalizeMode,NoiseSettings.type);
				}
				SetTexture(UNoise::GenerateTexture(NoiseMap,ColorCurve));
				double end = FPlatformTime::Seconds();
				UE_LOG(NoiseGenerator,Log,TEXT("NoiseExample::Regenerate() ==> Type: %s, Cycles: %d, RunTime: %fs"),*type,cycles,end-start);
			});	
		} else {
			SetTexture(UNoise::GenerateTexture(Center,TextureSize,&NoiseSettings,ColorCurve));
			double end = FPlatformTime::Seconds();
			UE_LOG(NoiseGenerator,Log,TEXT("NoiseExample::Regenerate() ==> Type: %s, Cycles: %d, RunTime: %fs"),*type,cycles,end-start);
		}
	}
}

void ANoiseExample::RandomSeed()
{
	NoiseSettings.seed = FMath::RandRange(-100000000,100000000);

	for(int i = 0; i < LayeredNoiseSettings.Num(); i++)	{
		LayeredNoiseSettings[i].LayerSettings.seed = NoiseSettings.seed;	
	}
	
	Regenerate();
}

void ANoiseExample::BeginPlay()
{
	Super::BeginPlay();
	Regenerate();
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
