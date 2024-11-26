#pragma once

class Poisson
{
protected:
	static bool IsPointValid(const FVector2D& point, const FVector2D& topLeft, const FVector2D& bottomRight, const TArray<FVector2D>& points, float minDist, float cellSize, TArray<TArray<int>>& grid);
public:
	static FVector2D GenerateRandomPointInAnnulus(const FVector2D& center, float minRadius, float maxRadius);
	static TArray<FVector2D> GeneratePoissonDiscSamples(const FVector2D& topLeft, const FVector2D& bottomRight, float minDist, int newPointsCount);
};
