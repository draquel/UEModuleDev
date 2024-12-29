#include "Character/BaseCharacter.h"
#include "InventoryComponent.h"
#include "Stats/StatsComponent.h"
#include "Damage/DamageSystemComponent.h"


ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	Stats = CreateDefaultSubobject<UStatsComponent>(TEXT("Stats"));
	DamageSystem = CreateDefaultSubobject<UDamageSystemComponent>(TEXT("DamageSystem"));
	DamageSystem->Stats = Stats;
	EquipmentSystem = CreateDefaultSubobject<UEquipmentSystemComponent>(TEXT("EquipmentSystem"));
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	DamageSystem->OnDeath.AddDynamic(this, &ABaseCharacter::Death);
	DamageSystem->OnBlock.AddDynamic(this, &ABaseCharacter::Block);
	DamageSystem->OnDamage.AddDynamic(this, &ABaseCharacter::DamageResponse);
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

FVector ABaseCharacter::GetForwardPos(int Range)
{
	return (GetActorForwardVector() * Range) + GetActorLocation();
}

void ABaseCharacter::SetRagDoll(bool Enable = true)
{
	GetMesh()->SetSimulatePhysics(Enable);
	GetMesh()->SetCollisionEnabled(Enable ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::QueryOnly);
}

float ABaseCharacter::GetHealth_Implementation()
{
	return Stats->GetBaseStat(health); 
}

float ABaseCharacter::GetMaxHealth_Implementation()
{
	return Stats->GetDerivedStat(max_health); 
}

float ABaseCharacter::Heal_Implementation(float Amount)
{
	return DamageSystem->Heal(Amount);
}

float ABaseCharacter::Damage_Implementation(FDamageInfo DamageInfo)
{
	return DamageSystem->Damage(DamageInfo); 
}

void ABaseCharacter::Death()
{
	IsAlive = false;
	SetRagDoll();
}

void ABaseCharacter::Block(bool CanBeParried)
{
	
}

void ABaseCharacter::DamageResponse(EDamageResponse Response)
{
	
}
