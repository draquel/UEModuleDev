#pragma once

#include "NoiseSettings.generated.h"

UENUM(BlueprintType)
enum NoiseType
{
	Perlin,
	Simplex,
	GPU_Perlin,
};

UENUM(BlueprintType)
enum NoiseFilter
{
	None,
	Rigid
};

USTRUCT(BlueprintType)
struct FNoiseSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<NoiseType> type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<NoiseFilter> filter;
	
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

	FNoiseSettings() 
	{
		type = Simplex;
		filter = None;
		scale = 0.00005f;
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