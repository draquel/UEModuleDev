// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemSystem/Public/Item.h"
#include "Components/ActorComponent.h"
#include "EquipmentSystemComponent.generated.h"


UENUM(BlueprintType)
enum EEquipmentSocket : uint8
{
	OneHand = 0,
	TwoHand = 1
};


USTRUCT(BlueprintType)
struct FWeaponSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FWeaponDefinition Definition;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool Active;

	FWeaponSlot() {
		Definition = FWeaponDefinition();
		Active = false;
	};

	FWeaponSlot(FWeaponDefinition definition)
	{
		Definition = definition;
		Active = false;
	}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CHARACTERSYSTEMS_API UEquipmentSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UEquipmentSystemComponent();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<TEnumAsByte<EEquipmentSocket>,FName> EquipmentSockets = { {OneHand,"spine_02Socket"}, {TwoHand,"clavicle_rSocket"} };
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	FName MainHandSocketName = FName("hand_rSocket");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TMap<TEnumAsByte<EEquipmentSocket>, UStaticMeshComponent*> EquipmentMeshComponents;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TMap<TEnumAsByte<EEquipmentSocket>,FWeaponSlot> EquipedWeapons;

	UFUNCTION(BlueprintCallable)
	void Equip(FWeaponDefinition WeaponDefinition);
	//
	UFUNCTION(BlueprintCallable)
	void Unequip(EEquipmentSocket Socket);
	
	UFUNCTION(BlueprintCallable)
	void Unsheath(EEquipmentSocket Socket);
	
	UFUNCTION(BlueprintCallable)
	void Sheath(EEquipmentSocket Socket);

	UFUNCTION(BlueprintCallable)
	bool HasActiveWeapon();

	UFUNCTION(BlueprintCallable)
	FWeaponDefinition GetActiveWeapon();

	UFUNCTION(BlueprintCallable)
	FWeaponDefinition GetSocketWeapon(EEquipmentSocket Socket);

	UFUNCTION(BlueprintCallable)
	bool IsSocketEquiped(EEquipmentSocket Socket);

	UFUNCTION(BlueprintCallable)
	bool IsSocketActive(EEquipmentSocket Socket);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void GenerateEquipmentSMCs(AActor* Owner, USkeletalMeshComponent* SKM);
};


