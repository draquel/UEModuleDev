// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Damage/Damagable.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UDamageSystemComponent;
class UStatsComponent;
class UInventoryComponent;

UCLASS()
class CHARACTERSYSTEMS_API ABaseCharacter : public ACharacter, public IDamageable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterComponents, meta = (AllowPrivateAccess = "true"))
	UInventoryComponent* Inventory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterComponents, meta = (AllowPrivateAccess = "true"))
	UStatsComponent* Stats;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterComponents, meta = (AllowPrivateAccess = "true"))
	UDamageSystemComponent* DamageSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	bool IsAlive = true;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = Character)
	FVector GetForwardPos(int Range = 250);

	UFUNCTION(BlueprintCallable, Category = Character)
	void SetRagDoll(bool Enable);

	UFUNCTION(BlueprintCallable, Category = Character)
	virtual float GetHealth_Implementation() override;
	UFUNCTION(BlueprintCallable, Category = Character)
	virtual float GetMaxHealth_Implementation() override;
	UFUNCTION(BlueprintCallable, Category = Character)
	virtual float Heal_Implementation(float Amount) override;
	UFUNCTION(BlueprintCallable, Category = Character)
	virtual float Damage_Implementation(FDamageInfo DamageInfo) override;

	UFUNCTION(BlueprintCallable, Category = Character)
	virtual void Death();
	UFUNCTION(BlueprintCallable, Category = Character)
	void Block(bool CanBeParried);
	UFUNCTION()
	void DamageResponse(EDamageResponse Response);
};
