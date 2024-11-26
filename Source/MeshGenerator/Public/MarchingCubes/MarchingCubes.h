#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MarchingCubes.generated.h"

UCLASS()
class MESHGENERATOR_API UMarchingCubes : public UObject
{
	GENERATED_BODY()

public:
	
	static TArray<FVector> cornerOffsets;
	
	static TArray<TArray<int>> triTable; 
	
	static TArray<TArray<int>> edgeConnections; 
};
