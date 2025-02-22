#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

USTRUCT(BlueprintType)
struct FItemSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 index;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FItemDefinition ItemDefinition;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FWeaponDetails WeaponDetails;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 count;

	FItemSlot()
	{
		ItemDefinition = FItemDefinition();
		WeaponDetails = FWeaponDetails();
		count = 0;
		index = 0;
	}
	
	bool IsEmpty() const { return count <= 0; }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLootDelegate);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ITEMSYSTEM_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxItems = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FItemSlot> Items;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool AddSlot(FItemSlot Slot);	
	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool AddItem(FItemDefinition Item, int32 SlotIndex = -1);
	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool AddWeapon(FItemDefinition Item, FWeaponDetails WeaponDetails, int32 SlotIndex = -1);
	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void DropItem(FItemDefinition Item);	

	UFUNCTION(BlueprintCallable, Category="Inventory")
	FItemSlot RemoveItem(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	FItemSlot RemoveNextItem();

	bool HasItem(FItemDefinition* Item);
	
	int32 FindItemSlot(FItemDefinition* Item);
	
	int32 FindEmptySlot();

	UFUNCTION(BlueprintCallable, Category="Inventory")
	int32 SlotsAvailable();

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool isSlotEmpty(int SlotIndex);
	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool isEmpty();

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool isFull();
	bool isSlotFull(int SlotIndex);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void DebugList();

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool Transfer(UInventoryComponent* Destination, int SlotIndex, int count);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool TransferAll(UInventoryComponent* Destination);
	
	UPROPERTY(BlueprintAssignable)
	FLootDelegate OnLoot;
	
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
