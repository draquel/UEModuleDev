#pragma once

#include "NoiseSettings.h"
#include "Noise.generated.h"

class FastNoiseLite;

USTRUCT()
struct FMinMax
{
	GENERATED_BODY()
	float min;
	float max;
	bool init = false;

	void Add(float in)
	{
		if(!init){
			min = in;
			max = in;
			init = true;
		} else {
			if (min > in){ min = in; }
			if (max < in){ max = in; }
		}
	}

	// Default constructor
	FMinMax() = default;

	// Copy constructor
	FMinMax(const FMinMax& Other) = default;

	// Move constructor
	FMinMax(FMinMax&& Other) noexcept = default;

	// Copy assignment
	FMinMax& operator=(const FMinMax& Other) = default;

	// Move assignment
	FMinMax& operator=(FMinMax&& Other) noexcept = default;
};

USTRUCT()
struct FNoiseMap2d
{
	GENERATED_BODY()

	UPROPERTY()	
	TMap<FIntVector2, float> Map;

	UPROPERTY()
	FIntVector Position;

	UPROPERTY()
	FIntVector2 Size;

	UPROPERTY()
	FMinMax MinMax;
	
	// Default constructor
	FNoiseMap2d()
	{
		Map.Empty();
		MinMax = FMinMax();
		Size = FIntVector2(64, 64);
		Position = FIntVector();
	}

	FNoiseMap2d(FIntVector InPosition, FIntVector2 InSize)
	{
		Map.Empty();
		MinMax = FMinMax();
		Size = InSize;
		Position = InPosition;
	}

	FNoiseMap2d(FIntVector InPosition, FIntVector2 InSize, TArray<float> InMap)
	{
		Map.Empty();
		MinMax = FMinMax();
		Size = InSize;
		Position = InPosition;
		
		for(int x = 0; x < Size.X; x++) {
			for(int y = 0; y < Size.Y; y++) {
				FIntVector2 index = FIntVector2(FMath::Floor(Position.X) + x, FMath::Floor(Position.Y) + y);
				Map.Add(index,InMap[y * Size.X + x]);
				MinMax.Add(Map[index]);
			}
		}
	}

	FNoiseMap2d(TMap<FIntVector2, float> InMap, FIntVector InPosition, FIntVector2 InSize, FMinMax InMinMax)
	{
		Map = InMap;
		Position = InPosition;
		Size = InSize;
		MinMax = InMinMax;
	}

	// Copy constructor
	FNoiseMap2d(const FNoiseMap2d& Other) = default;

	// Move constructor
	FNoiseMap2d(FNoiseMap2d&& Other) noexcept = default;

	// Copy assignment
	FNoiseMap2d& operator=(const FNoiseMap2d& Other) = default;

	// Move assignment
	FNoiseMap2d& operator=(FNoiseMap2d&& Other) noexcept = default;
};

USTRUCT()
struct FNoiseLayerData
{
	GENERATED_BODY();

	FNoiseMap2d noiseMap;

	UPROPERTY()
	float gain = 1.0f;

	UPROPERTY()
	UCurveFloat* curve = nullptr;


	FNoiseLayerData(){}
	
	FNoiseLayerData(FNoiseMap2d InNoiseMap, float InGain, UCurveFloat* InCurve)
	{
		noiseMap = InNoiseMap;
		gain = InGain;
		curve = InCurve;
	}
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
	static float EvaluateSlope(FVector pos, FNoiseSettings* NoiseSettings, int heightMultiplier);
	static float EvaluateHeightFalloff(FVector pos, FNoiseSettings* NoiseSettings, int heightMultiplier, int worldSize);

	static float Evaluate3D(FVector pos, FNoiseSettings* NoiseSettings);
	static float EvaluateDensity(FVector pos, FNoiseSettings* NoiseSettings, float densityMultiplier);

	static float EvaluateFilter(float noise, NoiseFilter filter);
	static float Normalize(float noise, float min, float max);
	static void Normalize(FNoiseMap2d& NoiseMap, NoiseNormalizeMode normalizeMode, NoiseType noiseType);

	static FNoiseMap2d GenerateMap2D(FIntVector pos, FIntVector2 mapSize, FNoiseSettings* NoiseSettings);
	static FNoiseMap2d GenerateMap2D(FIntVector pos, FIntVector2 mapSize, TArray<FLayeredNoiseSettings>* NoiseSettings);
	static void GenerateMap2D(FNoiseMap2d& NoiseMap,TArray<FNoiseLayerData>* layerData);
	
	static UTexture2D* GenerateTexture(FIntVector pos, FIntVector2 mapSize, FNoiseSettings* NoiseSettings, UCurveLinearColor* ColorCurve = nullptr);
	static UTexture2D* GenerateTexture(FIntVector pos, FIntVector2 mapSize, TArray<FLayeredNoiseSettings>* NoiseSettings, UCurveLinearColor* ColorCurve = nullptr);
	static UTexture2D* GenerateTexture(FNoiseMap2d* NoiseMap, UCurveLinearColor* ColorCurve = nullptr);

	static TMap<FIntVector,float> GenerateMap3D(FIntVector pos, FIntVector mapSize, FNoiseSettings* NoiseSettings);

	static FVector2D PoissonSample(const FVector2D& center, float minRadius, float maxRadius);
	static TArray<FVector2D> PoissonDiscSample(const FVector2D& topLeft, const FVector2D& bottomRight, float minDist, int newPointsCount);
};
