// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Pickupable.generated.h"

class UInventoryComponent;
class IInventoryCharacter;
// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UPickupable : public UInterface
{
	GENERATED_BODY()
};

class ITEMSYSTEM_API IPickupable
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Pickup(UInventoryComponent* InventoryComponent);

};
