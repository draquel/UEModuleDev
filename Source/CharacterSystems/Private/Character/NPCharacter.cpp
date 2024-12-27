// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/NPCharacter.h"
#include "InventoryComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
ANPCharacter::ANPCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);

	GetCharacterMovement()->GetNavMovementProperties()->bUseAccelerationForPaths = true;
}

// Called when the game starts or when spawned
void ANPCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ANPCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANPCharacter::Death()
{
	Super::Death();
	GetController()->UnPossess();
}

void ANPCharacter::Interact_Implementation(AActor* Interactor)
{
	// IInteractable::Interact_Implementation(Interactor);
	if(IsAlive)	{
		//Social
		UE_LOG(LogTemp, Warning, TEXT("NPC Says Hi!"));
	} else {
		//Loot
		Inventory->DebugList();
	}
}

bool ANPCharacter::CanInteract(AActor* Interactor) const
{
	return true;	
}

