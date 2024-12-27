// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageInfo.h"
#include "UObject/Interface.h"
#include "Damagable.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UDamageable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CHARACTERSYSTEMS_API IDamageable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetHealth();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetMaxHealth();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float Heal(float Amount);
	UFUNCTION(blueprintCallable, BlueprintNativeEvent)
	float Damage(FDamageInfo DamageInfo);
};
