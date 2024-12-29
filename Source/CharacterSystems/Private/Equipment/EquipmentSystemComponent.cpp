#include "Equipment/EquipmentSystemComponent.h"

#include "Character/BaseCharacter.h"

UEquipmentSystemComponent::UEquipmentSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UEquipmentSystemComponent::Equip(FWeaponDefinition WeaponDefinition)
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
	TArray<TEnumAsByte<EEquipmentSocket>> EquipmentMSockets;
	EquipmentMeshComponents.GetKeys(EquipmentMSockets);
	if (EquipmentMeshComponents.Contains(Socket)){
		EquipedWeapons.Add(Socket,FWeaponSlot(WeaponDefinition));
		UStaticMesh* mesh = EquipedWeapons[Socket].Definition.Item.Mesh;
		if (!mesh) {
			UE_LOG(LogTemp, Error, TEXT("Equiped weapons not found"));
		}else {
			EquipmentMeshComponents[Socket]->SetStaticMesh(mesh);
			EquipmentMeshComponents[Socket]->SetVisibility(true);		
		}
	} else {
		UE_LOG(LogTemp,Error,TEXT("EquipmentSystemComponent::Equip Error"));
	}
}

void UEquipmentSystemComponent::Unequip(EEquipmentSocket Socket)
{
	if (EquipmentMeshComponents.Contains(Socket))
	{
		EquipmentMeshComponents[Socket]->SetVisibility(false);
		EquipmentMeshComponents[Socket]->SetStaticMesh(nullptr);
		EquipedWeapons.Remove(Socket);	
	}
}

void UEquipmentSystemComponent::Unsheath(EEquipmentSocket Socket)
{
	EquipmentMeshComponents[Socket]->AttachToComponent(Cast<ABaseCharacter>(GetOwner())->GetMesh(),FAttachmentTransformRules::SnapToTargetNotIncludingScale,MainHandSocketName);
}

void UEquipmentSystemComponent::Sheath(EEquipmentSocket Socket)
{
	EquipmentMeshComponents[Socket]->AttachToComponent(Cast<ABaseCharacter>(GetOwner())->GetMesh(),FAttachmentTransformRules::SnapToTargetNotIncludingScale,EquipmentSockets[Socket]);
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
	UE_LOG(LogTemp,Error,TEXT("Socket is not Equipped"));
	return FWeaponDefinition();
}

bool UEquipmentSystemComponent::IsSocketEquiped(EEquipmentSocket Socket)
{
	return  EquipedWeapons.Num() > 0 && EquipedWeapons.Contains(Socket);
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

	ABaseCharacter* Owner = Cast<ABaseCharacter>(GetOwner());
	GenerateEquipmentSMCs(Owner,Owner->GetMesh());
}


void UEquipmentSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UEquipmentSystemComponent::GenerateEquipmentSMCs(AActor* Owner, USkeletalMeshComponent* SKM)
{
	for (auto socket : EquipmentSockets) {
		FName componentName = FName("StaticMeshComponent_"+FString::FromInt(socket.Key.GetIntValue()));
		UStaticMeshComponent* NewComponent = NewObject<UStaticMeshComponent>(Owner,UStaticMeshComponent::StaticClass(),componentName);
		NewComponent->SetFlags(RF_Transient);
		NewComponent->AttachToComponent(SKM,FAttachmentTransformRules::SnapToTargetNotIncludingScale,socket.Value);
		NewComponent->SetVisibility(false);
		NewComponent->RegisterComponent();
		
		EquipmentMeshComponents.Add(socket.Key, NewComponent);
	}
}