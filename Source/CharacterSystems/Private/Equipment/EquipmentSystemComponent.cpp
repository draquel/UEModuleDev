
#include "Equipment/EquipmentSystemComponent.h"

UEquipmentSystemComponent::UEquipmentSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UEquipmentSystemComponent::Equip(FWeaponDefinition WeaponDefinition, TMap<TEnumAsByte<EEquipmentSocket>, UStaticMeshComponent*> MeshComponents)
{
	TEnumAsByte<EEquipmentSocket> Socket;
	switch (WeaponDefinition.WeaponDetails.Type) {
		case One_Hand:
			Socket = OneHand;
			break;
		case Two_Hand:
			Socket = TwoHand;
			break;
		case Bow:
			Socket = TwoHand;
			break;
		default:
			Socket = OneHand;
			break;
	}
	EquipedWeapons.Add(Socket,FWeaponSlot(WeaponDefinition));
	MeshComponents[Socket]->SetStaticMesh(EquipedWeapons[Socket].Definition.Item.Mesh);
	MeshComponents[Socket]->SetVisibility(true);
}

void UEquipmentSystemComponent::Unequip(EEquipmentSocket Socket, TMap<TEnumAsByte<EEquipmentSocket>, UStaticMeshComponent*> MeshComponents)
{
	MeshComponents[Socket]->SetVisibility(false);
	MeshComponents[Socket]->SetStaticMesh(nullptr);
	EquipedWeapons.Remove(Socket);
}

void UEquipmentSystemComponent::Unsheath(FName SlotName)
{
	
}

void UEquipmentSystemComponent::Sheath(FName SlotName)
{
	
}

bool UEquipmentSystemComponent::HasActiveWeapon()
{
	if (!EquipedWeapons.IsEmpty()) {
		for (auto weapon : EquipedWeapons)	{
			if (weapon.Value.Active == true) {
				return true;
			}
		}
	}
	return false;
}

FWeaponDefinition UEquipmentSystemComponent::GetActiveWeapon()
{
	if (EquipedWeapons.Num() > 0) {
		for (auto weapon : EquipedWeapons)	{
			if (weapon.Value.Active) {
				return weapon.Value.Definition;
			}
		}
	}
	return FWeaponDefinition();	
}

FWeaponDefinition UEquipmentSystemComponent::GetSocketWeapon(EEquipmentSocket Socket)
{
	if (IsSocketEquiped(Socket)) {
		return EquipedWeapons.Find(Socket)->Definition;
	}
	return FWeaponDefinition();
}

bool UEquipmentSystemComponent::IsSocketEquiped(EEquipmentSocket Socket)
{
	return EquipedWeapons.Contains(Socket);
}

bool UEquipmentSystemComponent::IsSocketActive(EEquipmentSocket Socket)
{
	if (IsSocketEquiped(Socket)) {
		return EquipedWeapons.Find(Socket)->Active;
	}
	return false;
}

void UEquipmentSystemComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UEquipmentSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

