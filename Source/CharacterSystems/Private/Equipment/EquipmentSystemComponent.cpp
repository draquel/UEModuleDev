
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
	if (!MeshComponents.Contains(Socket)) {
		UE_LOG(LogTemp,Error,TEXT("no meshcomponent for socket: %i"),Socket.GetValue());
	}else {
		UE_LOG(LogTemp,Log,TEXT("meshcomponent for socket: %s"),*MeshComponents[Socket]->GetName());
	}
	MeshComponents[Socket]->SetStaticMesh(EquipedWeapons[Socket].Definition.Item.Mesh);
	MeshComponents[Socket]->SetVisibility(true);
	UE_LOG(LogTemp, Warning, TEXT("Equiped weapon: %s"),*EquipedWeapons[Socket].Definition.Item.Name.ToString());
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

void UEquipmentSystemComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UEquipmentSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

