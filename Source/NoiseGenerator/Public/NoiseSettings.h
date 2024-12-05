#pragma once

#include "NoiseSettings.generated.h"

UENUM(BlueprintType)
enum NoiseType
{
	None = 0 UMETA(Hidden),
	Perlin = 1,
	GPU_Perlin = 2,
	Simplex = 3,
	GPU_Simplex = 4
};

UENUM(BlueprintType)
enum NoiseFilter
{
	NoFilter = 0,
	Rigid = 1
};

UENUM(BlueprintType)
enum NoiseNormalizeMode
{
	NoNormalization	= 0,
	Local = 1,
	LocalPositive = 2,
	Global = 3,
	GlobalPositive = 4
};

USTRUCT(BlueprintType)
struct FNoiseSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<NoiseType> type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<NoiseFilter> filter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<NoiseNormalizeMode> normalizeMode;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta= (ClampMin=0.000001f))
	float scale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float seed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float frequency;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FVector offset;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int octaves;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float lacunarity;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float persistence;

	// UPROPERTY(EditAnywhere,BlueprintReadWrite)
	// bool normalize;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool domainWarping;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float domainWarpingScale;

	FNoiseSettings() 
	{
		type = Simplex;
		filter = NoFilter;
		normalizeMode = NoNormalization;
		
		scale = 5000.0f;
		seed = 12345;
		offset = FVector(1230, 3210, 3201);

		octaves = 3;
		lacunarity = 1.8f;
		persistence = 1;
		frequency = 1;

		domainWarping = false;
		domainWarpingScale = 1.0f;
	}

	bool operator==(const FNoiseSettings& other) const
	{
		if(
			type == other.type &&
			filter == other.filter &&
			normalizeMode == other.normalizeMode &&
			scale == other.scale &&
			seed == other.seed &&
			offset == other.offset &&
			octaves == other.octaves &&
			lacunarity == other.lacunarity &&
			persistence == other.persistence &&
			frequency == other.frequency &&
			domainWarping == other.domainWarping &&
			domainWarpingScale == other.domainWarpingScale
		) {	return true; }
		return false;
	}
};

USTRUCT(BlueprintType)
struct FLayeredNoiseSettings 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FNoiseSettings LayerSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* LayerCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Gain = 1.0f;

	FLayeredNoiseSettings()
	{
		LayerSettings = FNoiseSettings();
		LayerCurve = nullptr;
	}
};