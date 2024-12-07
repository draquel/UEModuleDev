#pragma once

#include "NoiseSettings.generated.h"

UENUM(BlueprintType)
enum NoiseSource
{
	NoSource = 0 UMETA(Hidden),
	CPU = 1,
	GPU = 2
};

UENUM(BlueprintType)
enum NoiseType
{
	NoType = 0 UMETA(Hidden),
	Perlin = 1,
	Simplex = 2
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
	TEnumAsByte<NoiseSource> source;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<NoiseType> type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<NoiseFilter> filter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<NoiseNormalizeMode> normalizeMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float gain;
	
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

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool domainWarping;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float domainWarpingScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* curve;

	FNoiseSettings() 
	{
		source = CPU;
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
			source == other.source &&
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

// USTRUCT(BlueprintType)
// struct FLayeredNoiseSettings 
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	FNoiseSettings Settings;
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	UCurveFloat* Curve;
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	float Gain = 1.0f;
//
// 	FLayeredNoiseSettings()
// 	{
// 		Settings = FNoiseSettings();
// 		Curve = nullptr;
// 	}
// 	FShaderNoiseSettings ToShaderSettings()
// 	{
// 		FShaderNoiseSettings settings;
// 		settings.Offset = (FVector3f)Settings.offset;
// 		settings.Octaves = Settings.octaves;
// 		settings.Frequency = Settings.frequency;
// 		settings.Lacunarity = Settings.lacunarity;
// 		settings.Persistence = Settings.persistence;
// 		settings.Scale = Settings.scale;
// 		settings.Filter = Settings.filter.GetValue();
// 		settings.Type = Settings.type.GetValue();
// 		settings.NormalizeMode = Settings.normalizeMode.GetValue();
// 		settings.DomainWarp = Settings.domainWarping ? Settings.domainWarpingScale : 0;
// 		return settings;
// 	}
// };

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