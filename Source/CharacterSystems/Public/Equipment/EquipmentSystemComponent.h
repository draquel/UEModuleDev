// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemSystem/Public/Item.h"
#include "Components/ActorComponent.h"
#include "EquipmentSystemComponent.generated.h"


UENUM()
enum EEquipmentSocket
{
	OneHand,
	TwoHand
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
	//										 onehand			two-hand / bow
	// const TArray<FName> EquipmentSockets = { "spine_02Socket", "clavicle_rSocket" };
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<TEnumAsByte<EEquipmentSocket>,FName> EquipmentSockets = { {OneHand,"spine_02Socket"}, {TwoHand,"clavicle_rSocket"} };

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	FName MainHandSocketName = FName("hand_rSocket");

	UEquipmentSystemComponent();
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TMap<TEnumAsByte<EEquipmentSocket>,FWeaponSlot> EquipedWeapons;

	UFUNCTION(BlueprintCallable)
	void Equip(FWeaponDefinition WeaponDefinition,TMap<TEnumAsByte<EEquipmentSocket>, UStaticMeshComponent*> MeshComponents);
	//
	UFUNCTION(BlueprintCallable)
	void Unequip(EEquipmentSocket Socket, TMap<TEnumAsByte<EEquipmentSocket>, UStaticMeshComponent*> MeshComponents);
	
	UFUNCTION(BlueprintCallable)
	void Unsheath(FName SlotName);
	
	UFUNCTION(BlueprintCallable)
	void Sheath(FName SlotName);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};


