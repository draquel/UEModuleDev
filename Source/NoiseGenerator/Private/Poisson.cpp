#include "Poisson.h"

FVector2D Poisson::GenerateRandomPointInAnnulus(const FVector2D& center, float minRadius, float maxRadius)
{
    float radius = FMath::FRandRange(minRadius, maxRadius);
    float angle = FMath::FRandRange(0.f, 2.f * PI);
    return FVector2D(center.X + radius * FMath::Cos(angle), center.Y + radius * FMath::Sin(angle));
}

bool Poisson::IsPointValid(const FVector2D& point, const FVector2D& topLeft, const FVector2D& bottomRight, const TArray<FVector2D>& points, float minDist, float cellSize, TArray<TArray<int>>& grid)
{
    if (point.X < topLeft.X || point.Y < topLeft.Y || point.X > bottomRight.X || point.Y > bottomRight.Y)
    {
        return false;
    }

    int cellX = (point.X - topLeft.X) / cellSize;
    int cellY = (point.Y - topLeft.Y) / cellSize;

    int startX = FMath::Max(0, cellX - 2);
    int startY = FMath::Max(0, cellY - 2);
    int endX = FMath::Min(grid.Num() - 1, cellX + 2);
    int endY = FMath::Min(grid[0].Num() - 1, cellY + 2);

    for (int x = startX; x <= endX; x++)
    {
        for (int y = startY; y <= endY; y++)
        {
            int pointIndex = grid[x][y];
            if (pointIndex != -1)
            {
                if (FVector2D::DistSquared(points[pointIndex], point) < minDist * minDist)
                {
                    return false;
                }
            }
        }
    }

    return true;
}

// Poisson Disc Sampling function
TArray<FVector2D> Poisson::GeneratePoissonDiscSamples(const FVector2D& topLeft, const FVector2D& bottomRight, float minDist, int newPointsCount)
{
    TArray<FVector2D> points;
    TQueue<FVector2D> activeList;
    float cellSize = minDist / FMath::Sqrt(2.0f);

    int gridWidth = FMath::CeilToInt((bottomRight.X - topLeft.X) / cellSize);
    int gridHeight = FMath::CeilToInt((bottomRight.Y - topLeft.Y) / cellSize);
    TArray<TArray<int>> grid;
    grid.SetNum(gridWidth,EAllowShrinking::No);

    for (int i = 0; i < gridWidth; i++)
    {
        grid[i].SetNum(gridHeight,EAllowShrinking::No);
        for (int j = 0; j < gridHeight; j++)
        {
            grid[i][j] = -1;
        }
    }

    FVector2D firstPoint = FVector2D(FMath::FRandRange(topLeft.X, bottomRight.X), FMath::FRandRange(topLeft.Y, bottomRight.Y));
    points.Add(firstPoint);
    activeList.Enqueue(firstPoint);

    int cellX = (firstPoint.X - topLeft.X) / cellSize;
    int cellY = (firstPoint.Y - topLeft.Y) / cellSize;
    grid[cellX][cellY] = 0;

    while (!activeList.IsEmpty())
    {
        FVector2D currentPoint;
        activeList.Dequeue(currentPoint);

        for (int i = 0; i < newPointsCount; i++)
        {
            FVector2D newPoint = Poisson::GenerateRandomPointInAnnulus(currentPoint, minDist, 2 * minDist);
            if (IsPointValid(newPoint, topLeft, bottomRight, points, minDist, cellSize, grid))
            {
                points.Add(newPoint);
                activeList.Enqueue(newPoint);
                int newCellX = (newPoint.X - topLeft.X) / cellSize;
                int newCellY = (newPoint.Y - topLeft.Y) / cellSize;
                grid[newCellX][newCellY] = points.Num() - 1;
            }
        }
    }

    return points;
}