// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.h"
#include "ItemManager.generated.h"

UCLASS()
class ITEMSYSTEM_API AItemManager : public AActor
{
	GENERATED_BODY()

public:
	AItemManager();

	UPROPERTY(EditAnywhere)
	TArray<AItemBase*> Items;

	UPROPERTY(EditAnywhere)
	FItemDefinition TestItemDefinition;
	
	UPROPERTY(EditAnywhere)
	UStaticMesh* DefaultMesh;

	UPROPERTY(EditAnywhere)
	UStaticMesh* DefaultContainerMesh;

	UFUNCTION(CallInEditor, meta=(Category="SceneItems"))
	void GetLevelItems();
	UFUNCTION(CallInEditor, meta=(Category="SceneItems"))
	void LoadItemDefinitions();
	UFUNCTION(CallInEditor, meta=(Category="SceneItems"))
	void ClearItemDefinitions();
	
	UFUNCTION(CallInEditor, meta=(Category="TestItem"))
	void LoadTestItem();
	UFUNCTION(CallInEditor, meta=(Category="TestItem"))
	void AddTestItem();
	UFUNCTION(CallInEditor, meta=(Category="TestItem"))
	void UpdateTestItem();
	
protected:
	// virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	FString DatabasePath = "Database/data.db3";

	UPROPERTY()
	USQLiteManager* SQLiteManager;

public:
	// virtual void Tick(float DeltaTime) override;
};
