#include "NoiseExample.h"
#include "Noise.h"

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
	UNoise::GenerateMap2D(Center,TextureSize,NoiseSettings,[this](FNoiseMap2d NoiseMap) {
		SetTexture(UNoise::GenerateTexture(&NoiseMap,ColorCurve));	
	});
}

void ANoiseExample::GenerateLayered()
{
	UNoise::GenerateMap2D(Center,TextureSize,LayeredNoiseSettings,[this](FNoiseMap2d NoiseMap) {
		SetTexture(UNoise::GenerateTexture(&NoiseMap,ColorCurve));	
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
