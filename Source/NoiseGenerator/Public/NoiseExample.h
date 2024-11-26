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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "NoiseExample")
	UStaticMeshComponent* RTMesh;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;

	UPROPERTY(EditAnywhere)
	UMaterial* DefaultMaterial;

	UPROPERTY(EditAnywhere)
	UTextureRenderTarget2D* RenderTarget;	

	// UPROPERTY(EditAnywhere)
	// bool AutoUpdate;

	UPROPERTY(EditAnywhere)
	FVector Center;

	UPROPERTY(EditAnywhere)
	bool UseLayeredNoise = false;

	UPROPERTY(EditAnywhere)
	FNoiseSettings NoiseSettings;

	UPROPERTY(EditAnywhere)
	TArray<FLayeredNoiseSettings> LayeredNoiseSettings;

	UPROPERTY(EditAnywhere)
	UCurveLinearColor* ColorCurve;
	
	UPROPERTY(EditAnywhere)
	FVector2D TextureSize = FVector2D(256,256);

	UFUNCTION(CallInEditor, meta=(Category="NoiseExample"))
	void Regenerate();

	UFUNCTION(CallInEditor, meta=(Category="NoiseExample"))
	void RandomSeed();
	
protected:
	virtual void BeginPlay() override;

private:
	// FNoiseSettings LastNoiseSettings;

	void Generate();

public:
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;
};
