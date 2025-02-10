#include "InventoryComponent.h"


UInventoryComponent::UInventoryComponent()
{
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	for(int32 i = 0; i < MaxItems; i++)	{
		FItemSlot Slot = FItemSlot();
		Slot.index = i;
		Items.Add(Slot);
	}
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UInventoryComponent::HasItem(FItemDefinition* Item)
{
	for (int i = 0; i < Items.Num(); i++) {
		if(Items[i].ItemDefinition.Name == Item->Name) {
			return true;	
		}
	}
	
	return false;
}

int32 UInventoryComponent::FindItemSlot(FItemDefinition* Item)
{
	for (int i = 0; i < Items.Num(); i++)	{
		if(Items[i].ItemDefinition.Name == Item->Name && Items[i].count < Items[i].ItemDefinition.StackSize) { return i; }
	}

	return FindEmptySlot();	
}

int32 UInventoryComponent::FindEmptySlot()
{
	bool found = false;
	int32 index = 0;
	for (int i = 0; i < Items.Num(); i++) {
		if(Items[i].IsEmpty())	{
			found = true;
			index = i;
			break;
		} 
	}

	return found ? index : -1;
}

int32 UInventoryComponent::SlotsAvailable()
{
	int32 count = 0;
	for (int i = 0; i < Items.Num(); i++) {
		if(Items[i].IsEmpty())	{
			count++;
		} 
	}

	return count;
}

bool UInventoryComponent::isSlotEmpty(int SlotIndex)
{
	if (SlotIndex < 0 || SlotIndex >= Items.Num()) { return true; }
	return Items[SlotIndex].IsEmpty() || Items[SlotIndex].count == 0;
}

bool UInventoryComponent::isEmpty()
{
	for (int i = 0; i < Items.Num(); i++)	{
		if(Items[i].count > 0){
			return false;
		}
	}

	return true;
}

bool UInventoryComponent::isFull()
{
	for (int i = 0; i < Items.Num(); i++)	{
		if(Items[i].count > 0){
			return true;
		}
	}

	return false;
}

void UInventoryComponent::DebugList()
{
	if(Items.Num() > 0)	{
		UE_LOG(LogTemp, Display, TEXT(" -- START Item List -- "));
	}
	int count = 0;
	for (FItemSlot ItemSlot : Items) {
		if(ItemSlot.IsEmpty()){ continue; }
		UE_LOG(LogTemp, Display, TEXT("=> %s x %d"), *ItemSlot.ItemDefinition.Name.ToString(), ItemSlot.count);
		count += ItemSlot.count;
	}
	if (Items.Num() > 0){
		UE_LOG(LogTemp, Display, TEXT(":: Total Count: %d"),count);
		UE_LOG(LogTemp, Display, TEXT(" -- END Item List -- "));
	}
}

bool UInventoryComponent::AddItem(FItemDefinition Item, int32 SlotIndex)
{
	if(SlotIndex < 0 || SlotIndex >= MaxItems){
		if(Item.Stackable && HasItem(&Item)){
			SlotIndex = FindItemSlot(&Item); 
		}else{
			SlotIndex = FindEmptySlot();
		}
	} else {
		if(Items[SlotIndex].count >= Items[SlotIndex].ItemDefinition.StackSize) {
			SlotIndex = FindEmptySlot();
		}
	}

	if(SlotIndex < 0)	{
		return false; //Too Full to Add
	}

	Items[SlotIndex].ItemDefinition = Item;
	Items[SlotIndex].count++;
	return true;
}

void UInventoryComponent::DropItem(FItemDefinition Item)
{
	AActor* Owner = GetOwner();
	FVector SpawnLocation = (Owner->GetActorForwardVector() * 200) + Owner->GetActorLocation();
	switch (Item.Type){
	case Weapon:
		{
			AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(AWeapon::StaticClass(),SpawnLocation,FRotator(),FActorSpawnParameters());
			Weapon->Definition = Item;
			Weapon->Id = Item.Id;
			Weapon->Update();
		}
		break;
	case Container:
		{
			AContainer* Container = GetWorld()->SpawnActor<AContainer>(AContainer::StaticClass(),SpawnLocation,FRotator(),FActorSpawnParameters());
			Container->Definition = Item;
			Container->Id = Item.Id;
			Container->Update();
		}
		break;
	default:
		{
			AItem* Spawn = GetWorld()->SpawnActor<AItem>(AItem::StaticClass(),SpawnLocation,FRotator(),FActorSpawnParameters());
			Spawn->Definition = Item;
			Spawn->Id = Item.Id;
			Spawn->Update();
		}
		break;
	}
}

FItemSlot UInventoryComponent::RemoveItem(int32 SlotIndex)
{
	if(Items[SlotIndex].count > 0) {
		Items[SlotIndex].count--;
	}
	FItemSlot Slot = Items[SlotIndex];
	if(Items[SlotIndex].count == 0) {
		Items[SlotIndex].ItemDefinition = FItemDefinition();
	}
	return Slot;
}

FItemSlot UInventoryComponent::RemoveNextItem()
{
	for (FItemSlot ItemSlot : Items) {
		if(!ItemSlot.IsEmpty()) {
			return RemoveItem(ItemSlot.index);
		}
	}
	return FItemSlot();
}
