#pragma once

#include "NoiseSettings.h"
#include "Noise.generated.h"

class FastNoiseLite;

USTRUCT()
struct FNoiseLayerData
{
	GENERATED_BODY();

	TMap<FVector2d, float> map;

	UPROPERTY()
	float gain = 1.0f;

	UPROPERTY()
	UCurveFloat* curve = nullptr;
};

UCLASS()
class NOISEGENERATOR_API UNoise : public UObject
{
	GENERATED_BODY()
protected:
	static FastNoiseLite SetupFastNoise(FNoiseSettings* NoiseSettings);
	
	static float FilterRigid(float noise);
public:
	static float Evaluate1D(float x, FNoiseSettings* NoiseSettings);
	
	static float Evaluate2D(FVector pos, FNoiseSettings* NoiseSettings);
	static float EvaluateHeight(FVector pos, FNoiseSettings* NoiseSettings, int heightMultiplier);
	static float EvaluateHeightFalloff(FVector pos, FNoiseSettings* NoiseSettings, int heightMultiplier, int worldSize);
	static float EvaluateSlope(FVector pos, FNoiseSettings* NoiseSettings, int heightMultiplier);

	static float Evaluate3D(FVector pos, FNoiseSettings* NoiseSettings);
	static float EvaluateDensity(FVector pos, FNoiseSettings* NoiseSettings, float densityMultiplier);

	static float EvaluateFilter(float noise, NoiseFilter filter);
	static float Normalize(float noise, float min, float max);

	static TMap<FVector2d,float> GenerateMap2D(FVector pos, FVector2d mapSize, FNoiseSettings* NoiseSettings);
	static TMap<FVector2d,float> GenerateMap2D(FVector pos, FVector2d mapSize,TArray<FLayeredNoiseSettings>* NoiseSettings);
	static UTexture2D* GenerateTexture(FVector pos, FVector2d mapSize, FNoiseSettings* NoiseSettings, UCurveLinearColor* ColorCurve = nullptr);
	static UTexture2D* GenerateTexture(FVector pos, FVector2d mapSize, TArray<FLayeredNoiseSettings>* NoiseSettings, UCurveLinearColor* ColorCurve = nullptr);
	static TMap<FVector,float> GenerateMap3D(FVector pos, FVector mapSize, FNoiseSettings* NoiseSettings);

	static FVector2D PoissonSample(const FVector2D& center, float minRadius, float maxRadius);
	static TArray<FVector2D> PoissonDiscSample(const FVector2D& topLeft, const FVector2D& bottomRight, float minDist, int newPointsCount);
};
