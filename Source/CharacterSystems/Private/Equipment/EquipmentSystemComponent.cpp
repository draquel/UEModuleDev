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

	if (EquipmentMeshComponents.Contains(Socket)){
		UStaticMesh* mesh = WeaponDefinition.Item.Mesh;
		if (!mesh) {
			UE_LOG(LogTemp, Error, TEXT("Equiped weapons not found"));
		}else {
			EquipedWeapons.Add(Socket,FWeaponSlot(WeaponDefinition));
			EquipmentMeshComponents[Socket]->SetStaticMesh(mesh);
			EquipmentMeshComponents[Socket]->SetVisibility(true);		
		}
	} else {
		UE_LOG(LogTemp,Error,TEXT("EquipmentSystemComponent::Socket %i does not have a valid Mesh Component"),Socket.GetIntValue());
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
	FName* DestinationSocket = &MainHandSocketName;
	if (GetSocketWeaponType(Socket) == Bow)	{
		DestinationSocket = &OffHandSocketName;
	}
	bool success = EquipmentMeshComponents[Socket]->AttachToComponent(SkeletalMeshComponent,FAttachmentTransformRules::SnapToTargetNotIncludingScale,*DestinationSocket);
	if (success) { EquipedWeapons[Socket].Active = true; }
	UE_LOG(LogTemp,Warning,TEXT("EquipmentSystemComponent::Unsheathe - %i, / %s / %i"),success, *DestinationSocket->ToString(),EquipmentMeshComponents.Num());
}

void UEquipmentSystemComponent::Sheath(EEquipmentSocket Socket)
{
	bool success = EquipmentMeshComponents[Socket]->AttachToComponent(SkeletalMeshComponent,FAttachmentTransformRules::SnapToTargetNotIncludingScale,EquipmentSockets[Socket]);
	if (success) { EquipedWeapons[Socket].Active = false; }
	UE_LOG(LogTemp,Warning,TEXT("EquipmentSystemComponent::Sheathe - %i, / %s / %i"),success, *EquipmentSockets[Socket].ToString(),EquipmentMeshComponents.Num());
}

void UEquipmentSystemComponent::ToggleEquipmentSocket(EEquipmentSocket Socket)
{
	if (IsSocketEquiped(Socket)) {
		if (IsSocketActive(Socket))	{
			EquipedWeapons[Socket].Active = false;
		} else {
			EquipedWeapons[Socket].Active = true;
		}
	}
}

bool UEquipmentSystemComponent::HasActiveWeapon()
{
	if (EquipedWeapons.Num() > 0) {
		for (auto weapon : EquipedWeapons)	{
			if (weapon.Value.Active == true) {
				// UE_LOG(LogTemp,Warning,TEXT("HasActiveWeapon - %s"),*weapon.Value.Definition.Item.Name.ToString());
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

EWeaponType UEquipmentSystemComponent::GetSocketWeaponType(EEquipmentSocket Socket)
{
	return GetSocketWeapon(Socket).WeaponDetails.Type;
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
}

void UEquipmentSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UEquipmentSystemComponent::GenerateEquipmentSMCs(AActor* Owner, USkeletalMeshComponent* SKM)
{
	OwnerActor = Owner;
	SkeletalMeshComponent = SKM;
	
	for (auto socket : EquipmentSockets) {
		FName componentName = FName("StaticMeshComponent_"+FString::FromInt(socket.Key.GetIntValue()));
		UStaticMeshComponent* NewComponent = NewObject<UStaticMeshComponent>(OwnerActor,UStaticMeshComponent::StaticClass(),componentName);
		NewComponent->SetFlags(RF_Transient);
		NewComponent->AttachToComponent(SkeletalMeshComponent,FAttachmentTransformRules::SnapToTargetNotIncludingScale,socket.Value);
		NewComponent->SetVisibility(false);
		NewComponent->RegisterComponent();
		
		EquipmentMeshComponents.Add(socket.Key, NewComponent);
	}
}