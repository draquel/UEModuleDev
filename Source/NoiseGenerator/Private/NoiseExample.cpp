#include "NoiseExample.h"
#include "Noise.h"
#include "NoiseGenerator.h"

ANoiseExample::ANoiseExample()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Texture Mesh");
	Mesh->SetupAttachment(RootComponent);
}

void ANoiseExample::Regenerate()
{
	if (NoiseSettings.Num() == 0){
		UE_LOG(NoiseGenerator, Error, TEXT("No NoiseSettings defined"));
		return;
	}
	UNoise::GenerateMap2D(Position,TextureSize,NoiseSettings,[this](FNoiseMap2d NoiseMap) {
		SetTexture(UNoise::GenerateTexture(&NoiseMap,ColorCurve));	
	});
}

void ANoiseExample::RandomSeed()
{
	for(int i = 0; i < NoiseSettings.Num(); i++)	{
		NoiseSettings[i].Settings.seed = FMath::RandRange(-100000000,100000000);	
	}	
	Regenerate();
}

void ANoiseExample::BeginPlay()
{
	Super::BeginPlay();
}

void ANoiseExample::SetTexture(UTexture2D* Texture)
{
	DynamicMaterial = UMaterialInstanceDynamic::Create(DefaultMaterial,this);
	DynamicMaterial->SetTextureParameterValue(FName("DynamicTexture"), Texture);
	Mesh->SetMaterial(0, DynamicMaterial);	
}

void ANoiseExample::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANoiseExample::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (!bHasBeenConstructed){
		bHasBeenConstructed = true;
		UE_LOG(NoiseGenerator, Display, TEXT("NoiseExample::OnConstruction() - Initialized"));
		return;
	}
	Regenerate();
}
