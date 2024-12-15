// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Noise.h"
#include "NoiseSettings.h"
#include "ProceduralMeshComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/Actor.h"
#include "MarchingCubes/MarchingCubes.h"
#include "MeshData/MeshData.h"
#include "QuadTree/QuadTree.h"
#include "QuadTree/QuadTreeSettings.h"
#include "MeshGeneratorExample.generated.h"


UENUM(BlueprintType)
enum EMeshExampleMode {
	Rect_2D = 0,
	QuadTree_2D = 1,
	MarchingCubes_3D = 2,
};

UENUM(BlueprintType)
enum E2DMeshExampleMode
{
	NoiseMap = 0,
	RenderTexture = 1
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
	UMaterial* Material;

	UPROPERTY(EditAnywhere)
	UTexture2D* NoiseTexture;
	
	UPROPERTY(EditAnywhere)
	float UVScale = 1.0f;
	
	UPROPERTY(EditAnywhere)
	TEnumAsByte<EMeshExampleMode> Mode;
	UPROPERTY(EditAnywhere)
	TEnumAsByte<E2DMeshExampleMode> DataMode2D;	
	UPROPERTY(EditAnywhere)
	FVector Size;
	UPROPERTY(EditAnywhere)
	int StepSize;

	UPROPERTY(EditAnywhere)
	int RectMesh_StepSize;
	
	UPROPERTY(EditAnywhere)
	bool autoUpdate = false;
	
	UPROPERTY(EditAnywhere)
	FMarchingCubesSettings MarchingCubesSettings;

	UPROPERTY(EditAnywhere)
	FQuadTreeSettings QuadTreeSettings;
	
	UPROPERTY(EditAnywhere)
	TArray<FNoiseSettings> NoiseSettings2D;

	UPROPERTY(EditAnywhere)
	TArray<FNoiseSettings> NoiseSettings3D;

	UPROPERTY(VisibleAnywhere)
	TArray<FNoiseSettings> LastNoiseSettings;

protected:
	
	UPROPERTY(Transient)
	bool bHasBeenConstructed = false;
	
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
	UTextureRenderTarget2D* CreateRenderTarget(ETextureRenderTargetFormat Format);
};


