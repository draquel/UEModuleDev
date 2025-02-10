// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Damage/Damagable.h"
#include "Equipment/EquipmentSystemComponent.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UDamageSystemComponent;
class UStatsComponent;
class UInventoryComponent;

UENUM(BlueprintType)
enum ECharacterMovementSpeed
{
	Idle,
	Walk,
	Jog,
	Sprint
};

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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BaseCharacter, meta = (AllowPrivateAccess = "true"))
	UInventoryComponent* Inventory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BaseCharacter, meta = (AllowPrivateAccess = "true"))
	UStatsComponent* Stats;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BaseCharacter, meta = (AllowPrivateAccess = "true"))
	UDamageSystemComponent* DamageSystem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BaseCharacter, meta = (AllowPrivateAccess = "true"))
	UEquipmentSystemComponent* EquipmentSystem;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BaseCharacter)
	bool IsAlive = true;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = BaseCharacter)
	FVector GetForwardPos(int Range = 250);

	UFUNCTION(BlueprintCallable, Category = BaseCharacter)
	void SetRagDoll(bool Enable);

	UFUNCTION(BlueprintCallable, Category = BaseCharacter)
	virtual float GetHealth_Implementation() override;
	UFUNCTION(BlueprintCallable, Category = BaseCharacter)
	virtual float GetMaxHealth_Implementation() override;
	UFUNCTION(BlueprintCallable, Category = BaseCharacter)
	virtual float Heal_Implementation(float Amount) override;
	UFUNCTION(BlueprintCallable, Category = BaseCharacter)
	virtual float Damage_Implementation(FDamageInfo DamageInfo) override;

	UFUNCTION(BlueprintCallable, Category = BaseCharacter)
	virtual void Death();
	UFUNCTION(BlueprintCallable, Category = BaseCharacter)
	void Block(bool CanBeParried);
	UFUNCTION(BlueprintCallable, Category = BaseCharacter)
	void DamageResponse(EDamageResponse Response);
};
