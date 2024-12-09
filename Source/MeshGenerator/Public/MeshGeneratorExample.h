// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Noise.h"
#include "NoiseSettings.h"
#include "ProceduralMeshComponent.h"
#include "GameFramework/Actor.h"
#include "MeshData/MeshData.h"
#include "QuadTree/QuadTreeSettings.h"
#include "MeshGeneratorExample.generated.h"


UENUM(BlueprintType)
enum EMeshExampleMode {
	Rect_2D = 0,
	QuadTree_2D = 1,
	MarchingCubes_3D = 2,
};

UCLASS()
class MESHGENERATOR_API AMeshGeneratorExample : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMeshGeneratorExample();

	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EMeshExampleMode> Mode;
	
	UPROPERTY(EditAnywhere)
	UMaterial* Material;
	UPROPERTY(EditAnywhere)
	float UVScale = 1.0f;
	
	UPROPERTY(EditAnywhere)
	FVector Size;
	UPROPERTY(EditAnywhere)
	int StepSize;
	
	UPROPERTY(EditAnywhere)
	float isoLevel = 0.5f;
	UPROPERTY(EditAnywhere)
	bool interpolate = false;
	UPROPERTY(EditAnywhere)
	bool showSides = false;

	UPROPERTY(EditAnywhere)
	FQuadTreeSettings QuadTreeSettings;
	
	UPROPERTY(EditAnywhere)
	bool autoUpdate = false;

	UPROPERTY(EditAnywhere)
	TArray<FNoiseSettings> NoiseSettings2D;

	UPROPERTY(EditAnywhere)
	TArray<FNoiseSettings> NoiseSettings3D;
	// FNoiseSettings NoiseSettings = FNoiseSettings(D3);

	UPROPERTY(Transient)
	bool bHasBeenConstructed = false;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(CallInEditor,meta=(Category="MeshGeneratorExample"))
	void Generate();
	void Regenerate();

	void UpdateMesh(FMeshData* Data);
	
	FNoiseMap2d* NoiseMap2D;
	FNoiseMap3d* NoiseMap3D;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(CallInEditor,meta=(Category="MeshGeneratorExample"))
	void DebugDraw();
	void DebugDrawBounds(float life, FColor color);
	void DebugDrawPositionMarker(float life, FColor color);

	UFUNCTION(CallInEditor,meta=(Category="MeshGeneratorExample"))
	void Clear();
	FVector GetPlayerPos() const;
};


