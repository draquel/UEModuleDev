#pragma once

#include "MinMax.generated.h"

USTRUCT()
struct TOOLSLIB_API FMinMax 
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	float min = 0;
	UPROPERTY(EditAnywhere)
	float max = 0;
protected:
	bool init = false;

public:
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

	FMinMax(float in_min, float in_max)
	{
		min = in_min;
		max = in_max;
		init = true;
	}

	// Copy constructor
	FMinMax(const FMinMax& Other) = default;

	// Move constructor
	FMinMax(FMinMax&& Other) noexcept = default;

	// Copy assignment
	FMinMax& operator=(const FMinMax& Other) = default;

	// Move assignment
	FMinMax& operator=(FMinMax&& Other) noexcept = default;

	FVector2D ToVector2D() const {
		return FVector2D(min, max);
	}
};