// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SQLiteManager.h"
#include "Engine/GameInstance.h"
#include "UDatabaseGameInstance.generated.h"


UCLASS(BlueprintType, Blueprintable)
class DATABASEMANAGER_API UDatabaseGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UDatabaseGameInstance();

	UPROPERTY(EditAnywhere)
	FSQLiteSettings DatabaseSettings;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USQLiteManager* DatabaseManager;

	virtual void Init() override;

	virtual void BeginDestroy() override;
};
