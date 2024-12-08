// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NoiseSettings.h"
#include "ProceduralMeshComponent.h"
#include "GameFramework/Actor.h"
#include "MeshGeneratorExample.generated.h"

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
	FVector Size;
	UPROPERTY(EditAnywhere)
	float isoLevel = 0.5f;
	UPROPERTY(EditAnywhere)
	bool interpolate = false;
	UPROPERTY(EditAnywhere)
	bool showSides = false;
	UPROPERTY(EditAnywhere)
	float UVScale = 1.0f;

	UPROPERTY(EditAnywhere)
	FNoiseSettings NoiseSettings = FNoiseSettings(D3);
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(CallInEditor,meta=(Category="MeshGeneratorExample"))
	void Generate();

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
};


