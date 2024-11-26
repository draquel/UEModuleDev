#include "NoiseExample.h"
#include "Noise.h"
#include "NoiseGenerator.h"
#include "Engine/TextureRenderTarget2D.h"
#include "NoiseGenerator/Public/PerlinNoiseShader/PerlinNoiseShader.h"

ANoiseExample::ANoiseExample()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	TMesh = CreateDefaultSubobject<UStaticMeshComponent>("Texture Mesh");
	TMesh->SetupAttachment(RootComponent);
	RTMesh = CreateDefaultSubobject<UStaticMeshComponent>("Render Target Mesh");
	RTMesh->SetupAttachment(RootComponent);

	RenderTarget = CreateDefaultSubobject<UTextureRenderTarget2D>("RenderTarget");
}

void ANoiseExample::Generate()
{
	Regenerate();
}

void ANoiseExample::Regenerate()
{
	double start = FPlatformTime::Seconds();
	FString type = TEXT("NONE");
	if (NoiseSettings.type == NoiseType::GPU_Perlin) {
		type = TEXT("GPU Noise");
		TMesh->SetVisibility(false);
		RTMesh->SetVisibility(true);

		RenderTarget->SizeX = TextureSize.X;
		RenderTarget->SizeY = TextureSize.Y;

		UPerlinNoiseShaderLibrary::ExecuteRTComputeShader(RenderTarget,(FVector3f)Center,(FVector3f)NoiseSettings.offset,NoiseSettings.octaves,NoiseSettings.lacunarity,NoiseSettings.persistence,NoiseSettings.scale);
	}else {
		type = TEXT("CPU Noise");
		TMesh->SetVisibility(true);
		RTMesh->SetVisibility(false);
		DynamicMaterial = UMaterialInstanceDynamic::Create(DefaultMaterial,this);
		UTexture2D* Texture;
		if (UseLayeredNoise) {
			Texture	= UNoise::GenerateTexture(Center,TextureSize,&LayeredNoiseSettings,ColorCurve);
		}else{
			Texture = UNoise::GenerateTexture(Center,TextureSize,&NoiseSettings,ColorCurve);
		}
		DynamicMaterial->SetTextureParameterValue(FName("DynamicTexture"), Texture);
		TMesh->SetMaterial(0, DynamicMaterial);
	}
	double end = FPlatformTime::Seconds();
	int cycles = TextureSize.X * TextureSize.Y;
	if (UseLayeredNoise){ for (auto Layer : LayeredNoiseSettings){ cycles *= Layer.LayerSettings.octaves; }	}
	else{ cycles *= NoiseSettings.octaves; }
	UE_LOG(NoiseGenerator,Log,TEXT("NoiseExample::Regenerate() ==> Type: %s, Cycles: %d, RunTime: %fs"),*type,cycles,end-start);	
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
	Generate();
}

void ANoiseExample::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANoiseExample::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	Generate();
}
