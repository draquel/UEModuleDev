#include "NoiseExample.h"
#include "Noise.h"
#include "NoiseGenerator.h"
#include "Engine/TextureRenderTarget2D.h"
#include "NoiseGenerator/Public/NoiseTextureComputeShader/NoiseTextureComputeShader.h"

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
	switch (Mode)
	{
		default:
		case Data:
			UNoise::GenerateMap2D(Position,TextureSize,StepSize,NoiseSettings, [this](FNoiseMap2d NoiseMap) {
				SetTexture(UNoise::GenerateTexture(&NoiseMap,ColorCurve));	
			});
			break;
		case Texture:
			UTextureRenderTarget2D* RT = CreateRenderTarget(TextureSize/StepSize, RTF_R16f);
			FNoiseTextureComputeShaderDispatchParams Params = FNoiseTextureComputeShaderInterface::BuildParams(RT,(FVector3f)GetActorLocation(),FVector3f(TextureSize.X/StepSize,TextureSize.Y/StepSize,1),StepSize,NoiseSettings);
			FNoiseTextureComputeShaderInterface::Dispatch(Params,[this,RT]()	{
				UTexture2D* texture = RT->ConstructTexture2D(GetWorld(), "SavedDataTexture", RF_Public | RF_Standalone);
				SetTexture(texture);
			});
			break;
	}
	
}

void ANoiseExample::RandomSeed()
{
	for(int i = 0; i < NoiseSettings.Num(); i++)	{
		NoiseSettings[i].seed = FMath::RandRange(-100000000,100000000);	
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

UTextureRenderTarget2D* ANoiseExample::CreateRenderTarget(FIntVector2 Size, ETextureRenderTargetFormat Format)
{
	// Create the render target
	UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>();
    
	// Set the render target properties
	RenderTarget->RenderTargetFormat = Format;
	RenderTarget->InitAutoFormat(Size.X, Size.Y);
	RenderTarget->ClearColor = FLinearColor::Black;
	RenderTarget->bAutoGenerateMips = false;
	RenderTarget->UpdateResourceImmediate(true);

	return RenderTarget;
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
	if (Mode == Data) {
		Regenerate();
	}
}
