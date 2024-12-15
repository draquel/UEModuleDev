#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MarchingCubes.generated.h"

USTRUCT(BlueprintType)
struct FMarchingCubesSettings
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float iso;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool interpolate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool renderSides;
	
public:
	
	FMarchingCubesSettings()
	{
		iso = 0.5f;
		interpolate = false;
		renderSides = false;
	}

	FMarchingCubesSettings(float _iso, bool _interpolate, bool _renderSides)
	{
		iso = _iso;
		interpolate = _interpolate;
		renderSides = _renderSides;
	}
	
};

UCLASS()
class MESHGENERATOR_API UMarchingCubes : public UObject
{
	GENERATED_BODY()

public:
	
	static TArray<FVector> cornerOffsets;
	
	static TArray<TArray<int>> triTable; 
	
	static TArray<TArray<int>> edgeConnections; 
};
