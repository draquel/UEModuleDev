#pragma once

#include "NoiseSettings.generated.h"

UENUM(BlueprintType)
enum NoiseMode
{
	NoMode = 0 UMETA(Hidden),
	D1 = 1,
	D2 = 2,
	D3 = 3
};

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

UENUM(BlueprintType)
enum NoiseDensityFunction
{
	NoDensityFunction = 0,
	Floor = 1,
	Sphere = 2
};

USTRUCT(BlueprintType)
struct FShaderNoiseSettingsA
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector4 Offset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector4 General;
};

USTRUCT(BlueprintType)
struct FShaderNoiseSettingsB
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector4 FBM;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector4 Options;
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

	explicit FNoiseSettings() 
	{
		source = CPU;
		type = Simplex;
		filter = NoFilter;
		normalizeMode = NoNormalization;

		gain = 1.0f;
		scale = 5000.0f;
		seed = 12345;
		offset = FVector(1230, 3210, 3201);

		octaves = 3;
		lacunarity = 1.8f;
		persistence = 1;
		frequency = 1;

		domainWarping = false;
		domainWarpingScale = 1.0f;

		curve = nullptr;
	}

	bool operator==(const FNoiseSettings& other) const
	{
		if(
			source == other.source &&
			type == other.type &&
			filter == other.filter &&
			normalizeMode == other.normalizeMode &&
			gain == other.gain &&
			scale == other.scale &&
			seed == other.seed &&
			offset == other.offset &&
			octaves == other.octaves &&
			lacunarity == other.lacunarity &&
			persistence == other.persistence &&
			frequency == other.frequency &&
			domainWarping == other.domainWarping &&
			domainWarpingScale == other.domainWarpingScale &&
			curve == other.curve
		) {	return true; }
		return false;
	}

	static bool Compare(TArray<FNoiseSettings> a, TArray<FNoiseSettings> b)
	{
		if (a.Num() == b.Num()) {
			for (int i = 0; i < a.Num(); i++) {
				if (a[i] != b[i]){ return false; }
			}
			return true;
		}
		return false;
	}
};

USTRUCT(BlueprintType)
struct FShaderNoiseSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector3f Offset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Octaves;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Frequency;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Lacunarity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Persistence;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Scale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Filter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NormalizeMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DomainWarp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Gain;

	FShaderNoiseSettings()
	{
		Type = Simplex;
		Filter = NoFilter;
		NormalizeMode = NoNormalization;
		Gain = 1.0f;
		Scale = 5000.0f;
		Offset = FVector3f(1230, 3210, 3201);
		Octaves = 3;
		Lacunarity = 1.8f;
		Persistence = 1;
		Frequency = 1;
		DomainWarp = 1.0f;
	}
	
	FShaderNoiseSettings(FNoiseSettings* NoiseSettings)
	{
		Offset = (FVector3f)NoiseSettings->offset;
		Octaves = NoiseSettings->octaves;
		Frequency = NoiseSettings->frequency;
		Lacunarity = NoiseSettings->lacunarity;
		Persistence = NoiseSettings->persistence;
		Scale = NoiseSettings->scale;
		Filter = NoiseSettings->filter.GetValue();
		Type = NoiseSettings->type.GetValue();
		NormalizeMode = NoiseSettings->normalizeMode.GetValue();
		DomainWarp = NoiseSettings->domainWarping ? NoiseSettings->domainWarpingScale : 0;
		Gain = NoiseSettings->gain;
	}
};
