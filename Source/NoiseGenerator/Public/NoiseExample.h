#pragma once

#include "CoreMinimal.h"
#include "NoiseSettings.h"
#include "GameFramework/Actor.h"
#include "NoiseExample.generated.h"

UCLASS()
class NOISEGENERATOR_API ANoiseExample : public AActor
{
	GENERATED_BODY()

public:
	ANoiseExample();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "NoiseExample")
	UStaticMeshComponent* TMesh;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;

	UPROPERTY(EditAnywhere)
	UMaterial* DefaultMaterial;

	UPROPERTY(EditAnywhere)
	FIntVector Center;

	UPROPERTY(EditAnywhere)
	bool UseLayeredNoise = false;

	UPROPERTY(EditAnywhere)
	FNoiseSettings NoiseSettings;

	UPROPERTY(EditAnywhere)
	TArray<FLayeredNoiseSettings> LayeredNoiseSettings;

	UPROPERTY(EditAnywhere)
	UCurveLinearColor* ColorCurve;
	
	UPROPERTY(EditAnywhere)
	FIntVector2 TextureSize = FIntVector2(256,256);

	UFUNCTION(CallInEditor, meta=(Category="NoiseExample"))
	void Regenerate();

	UFUNCTION(CallInEditor, meta=(Category="NoiseExample"))
	void RandomSeed();
	
	
protected:
	virtual void BeginPlay() override;

	void SetTexture(UTexture2D* Texture);

public:
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;
};
