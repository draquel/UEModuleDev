#pragma once

#include "NoiseGenerator.h"
#include "NoiseSettings.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Structs/MinMax.h"
#include "Noise.generated.h"

class FastNoiseLite;

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
	int StepSize;

	UPROPERTY()
	FMinMax MinMax;
	
	// Default constructor
	FNoiseMap2d()
	{
		Map.Empty();
		MinMax = FMinMax();
		Size = FIntVector2(64, 64);
		StepSize = 1;
		Position = FIntVector();
	}

	FNoiseMap2d(FIntVector InPosition, FIntVector2 InSize, int InStepSize)
	{
		Map.Empty();
		MinMax = FMinMax();
		Size = InSize;
		StepSize = InStepSize;
		Position = InPosition;
	}

	FNoiseMap2d(FIntVector InPosition, FIntVector2 InSize, int InStepSize, TArray<float> InMap)
	{
		Map.Empty();
		MinMax = FMinMax();
		Size = InSize;
		StepSize = InStepSize;
		Position = InPosition;
		FIntVector2 scaledSize = InSize / InStepSize;

		for(int x = 0; x < scaledSize.X; x++) {
			for(int y = 0; y < scaledSize.Y; y++) {
				FIntVector2 mp = FIntVector2(x, y);
				FIntVector2 index = FIntVector2(Position.X, Position.Y) + mp * StepSize;
				Map.Add(index,InMap[mp.X + (mp.Y * scaledSize.X)]);
				MinMax.Add(Map[index]);
			}
		}
	}

	FNoiseMap2d(TMap<FIntVector2, float> InMap, FIntVector InPosition, FIntVector2 InSize, int InStepSize, FMinMax InMinMax)
	{
		Map = InMap;
		Position = InPosition;
		Size = InSize;
		StepSize = InStepSize;
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
struct FNoiseLayer2DData
{
	GENERATED_BODY();

	FNoiseMap2d noiseMap;

	UPROPERTY()
	float gain = 1.0f;

	UPROPERTY()
	UCurveFloat* curve = nullptr;


	FNoiseLayer2DData(){}
	
	FNoiseLayer2DData(FNoiseMap2d InNoiseMap, float InGain, UCurveFloat* InCurve)
	{
		noiseMap = InNoiseMap;
		gain = InGain;
		curve = InCurve;
	}
};

USTRUCT()
struct FNoiseMap3d
{
	GENERATED_BODY()

	UPROPERTY()	
	TMap<FIntVector, float> Map;

	UPROPERTY()
	FIntVector Position;

	UPROPERTY()
	FIntVector Size;

	UPROPERTY()
	int StepSize;

	UPROPERTY()
	FMinMax MinMax;
	
	// Default constructor
	FNoiseMap3d()
	{
		Map.Empty();
		MinMax = FMinMax();
		Size = FIntVector(64, 64,64);
		StepSize = 1;
		Position = FIntVector();
	}

	FNoiseMap3d(FIntVector InPosition, FIntVector InSize,int InStepSize = 1)
	{
		Map.Empty();
		MinMax = FMinMax();
		Size = InSize;
		StepSize = InStepSize;
		Position = InPosition;
	}

	FNoiseMap3d(FIntVector InPosition, FIntVector InSize,int InStepSize,TArray<float> InMap)
	{
		Map.Empty();
		MinMax = FMinMax();
		Size = InSize;
		StepSize = InStepSize;
		Position = InPosition;
		FIntVector scaledSize = InSize / InStepSize;
		
		for(int x = 0; x < scaledSize.X; x++) {
			for(int y = 0; y < scaledSize.Y; y++) {
				for(int z = 0; z < scaledSize.Z; z++)	{
					FIntVector mp = FIntVector(x, y, z);
					FIntVector index = Position + mp * StepSize;
					Map.Add(index,InMap[mp.X + (mp.Y * scaledSize.X) + (mp.Z * scaledSize.X * scaledSize.Y)]);
					MinMax.Add(Map[index]);
				}
			}
		}
	}

	FNoiseMap3d(TMap<FIntVector3, float> InMap, FIntVector InPosition, FIntVector3 InSize, int InStepSize, FMinMax InMinMax)
	{
		Map = InMap;
		Position = InPosition;
		Size = InSize;
		StepSize = InStepSize;
		MinMax = InMinMax;
	}

	// Copy constructor
	FNoiseMap3d(const FNoiseMap3d& Other) = default;

	// Move constructor
	FNoiseMap3d(FNoiseMap3d&& Other) noexcept = default;

	// Copy assignment
	FNoiseMap3d& operator=(const FNoiseMap3d& Other) = default;

	// Move assignment
	FNoiseMap3d& operator=(FNoiseMap3d&& Other) noexcept = default;
};

USTRUCT()
struct FNoiseLayer3DData
{
	GENERATED_BODY();

	FNoiseMap3d noiseMap;

	UPROPERTY()
	float gain = 1.0f;

	UPROPERTY()
	UCurveFloat* curve = nullptr;

	FNoiseLayer3DData(){}
	
	FNoiseLayer3DData(FNoiseMap3d InNoiseMap, float InGain, UCurveFloat* InCurve)
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

	static FMinMax GetTypeMinMax(NoiseType type);

public:
	//CPU NOISE Evaluation
	static float Evaluate1D(float x, FNoiseSettings* NoiseSettings);
	static float Evaluate2D(FVector pos, FNoiseSettings* NoiseSettings);
	static float Evaluate3D(FVector pos, FNoiseSettings* NoiseSettings);
	
	static float Normalize(float noise, float min, float max);
	static float Normalize(NoiseNormalizeMode normalizeMode, float noise, FMinMax localMinMax, FMinMax typeMinMax);
	static void Normalize(FNoiseMap2d* NoiseMap, NoiseNormalizeMode normalizeMode, NoiseType noiseType);
	static void Normalize(FNoiseMap3d* NoiseMap, NoiseNormalizeMode normalizeMode, NoiseType noiseType);

	static float EvaluateFilter(float noise, NoiseFilter filter);

	//CPU 2D Maps
	static FNoiseMap2d GenerateMap2D(FIntVector pos, FIntVector2 mapSize, int stepSize, FNoiseSettings* NoiseSettings);
	static FNoiseMap2d GenerateMap2D(FIntVector pos, FIntVector2 mapSize, int stepSize, TArray<FNoiseSettings>* NoiseSettings);
	static void GenerateMap2D(FNoiseMap2d& NoiseMap,TArray<FNoiseLayer2DData>* layerData);

	//Needs unique Name -- Accessors for CPU & GPU Generation, with callback to handle Async uniformily.
	static void GenerateMap2D(FIntVector pos, FIntVector2 mapSize, int stepSize, FNoiseSettings NoiseSettings, TFunction<void(FNoiseMap2d* NoiseMap)> Callback);
	static void GenerateMap2D(FIntVector pos, FIntVector2 mapSize, int stepSize, TArray<FNoiseSettings> NoiseSettings, TFunction<void(FNoiseMap2d* NoiseMap)> Callback);
	static UTexture2D* GenerateDataTexture(FNoiseMap2d* NoiseMap, FName name);

	//2D TEXTURE Generators
	static UTexture2D* GenerateTexture(FIntVector pos, FIntVector2 mapSize, FNoiseSettings* NoiseSettings, UCurveLinearColor* ColorCurve = nullptr);
	static UTexture2D* GenerateTexture(FIntVector pos, FIntVector2 mapSize, TArray<FNoiseSettings>* NoiseSettings, UCurveLinearColor* ColorCurve = nullptr);
	static UTexture2D* GenerateTexture(FNoiseMap2d* NoiseMap, UCurveLinearColor* ColorCurve = nullptr);
	static void GenerateTexture(UTextureRenderTarget2D* RenderTarget, FIntVector pos, FIntVector2 Size, int stepSize, TArray<FNoiseSettings>* NoiseSettings, TFunction<void()> Callback);
	static void GenerateTexture(UTexture2D* Texture, FIntVector pos, FIntVector2 mapSize, int StepSize, TArray<FNoiseSettings> NoiseSettings, TFunction<void(UTexture2D* Texture2D)> Callback);

	//CPU 3D Maps
	static FNoiseMap3d GenerateMap3D(FIntVector pos, FIntVector mapSize, int stepSize, FNoiseSettings* NoiseSettings, NoiseDensityFunction DensityFunction = NoDensityFunction);
	static void GenerateMap3D(FNoiseMap3d& NoiseMap,TArray<FNoiseLayer3DData>* layerData);
	
	//Needs unique Name -- Accessors for CPU & GPU Generation, with callback to handle Async uniformily.
	static void GenerateMap3D(FIntVector pos, FIntVector mapSize, int stepSize, FNoiseSettings NoiseSettings, NoiseDensityFunction DensityFunction, TFunction
	                          <void(FNoiseMap3d* NoiseMap)> Callback);
	static void GenerateMap3D(FIntVector pos, FIntVector mapSize, int stepSize, TArray<FNoiseSettings> NoiseSettings, NoiseDensityFunction
	                          DensityFunction, TFunction<void(FNoiseMap3d* NoiseMap)> Callback);
	
	//3D TEXTURE Generators
	static UVolumeTexture* GenerateTexture(FNoiseMap3d* NoiseMap, UCurveLinearColor* ColorCurve);
	//Poisson
	static FVector2D PoissonSample(const FVector2D& center, float minRadius, float maxRadius);
	static TArray<FVector2D> PoissonDiscSample(const FVector2D& topLeft, const FVector2D& bottomRight, float minDist, int newPointsCount);
	static UTextureRenderTarget2D* CreateRenderTarget(FIntVector2 Size, ETextureRenderTargetFormat Format);
};
