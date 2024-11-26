// Fill out your copyright notice in the Description page of Project Settings.


#include "Stats/StatsComponent.h"


// Sets default values for this component's properties
UStatsComponent::UStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	Defaults.InitializeAll();
	Stats.InitializeAll();
}


void UStatsComponent::BeginPlay()
{
	Super::BeginPlay();
	Stats.CopyFrom(Defaults);
	UpdateDerivedStats();
	Stats.BaseStats[health] = Stats.DerivedStats[max_health];
	Stats.BaseStats[stamina] = Stats.DerivedStats[max_stamina];
	Stats.BaseStats[magic] = Stats.DerivedStats[max_magic];
}

void UStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

float UStatsComponent::GetBaseStat(EBaseStats stat)
{
	if(Stats.BaseStats.Contains(stat)) {
		return Stats.BaseStats[stat];
	}
	UE_LOG(LogTemp, Fatal, TEXT("Base Stat:  %d, not found"),stat);
	return 0;
}

float UStatsComponent::GetDerivedStat(EDerivedStats stat)
{
	if(Stats.DerivedStats.Contains(stat))	{
		return Stats.DerivedStats[stat];
	}
	UE_LOG(LogTemp, Fatal, TEXT("Derived Stat:  %d, not found"),stat);
	return 0;
}

void UStatsComponent::SetBaseStat(EBaseStats stat, float value)
{
	if(value >= 0) {
		Stats.BaseStats[stat] = value;
	}else{
		UE_LOG(LogTemp, Warning, TEXT("Stat value can not be negative."));
	}
}

void UStatsComponent::SetDerivedStat(EDerivedStats stat, float value)
{
	if(value >= 0) {
		Stats.DerivedStats[stat] = value;
	}else {
		UE_LOG(LogTemp, Warning, TEXT("Stat value can not be negative."));
	}
}

void UStatsComponent::UpdateDerivedStats()
{
	float statMod = 10;
	Stats.DerivedStats[max_health] = Stats.BaseStats[constitution] * statMod;	
	Stats.DerivedStats[max_stamina] = Stats.BaseStats[dexterity] * statMod;	
	Stats.DerivedStats[max_magic] = Stats.BaseStats[wisdom] * statMod;	
	Stats.DerivedStats[attack_power] = Stats.BaseStats[strength] * statMod;	
	Stats.DerivedStats[attack_speed] = statMod / Stats.BaseStats[dexterity];	
	Stats.DerivedStats[spell_power] = Stats.BaseStats[wisdom] * statMod;	
	Stats.DerivedStats[movement_speed] = 400 + 100 * ((Stats.BaseStats[strength]+Stats.BaseStats[dexterity])/50);
}

FStatus UStatsComponent::GetStatus()
{
	return CalculateStats();
}

FStatus UStatsComponent::CalculateEffects()
{
	FStatus totalEffect = FStatus();
	
	for (FStatusEffect StatusEffect : Effects) {
		for (EBaseStats stat : TEnumRange<EBaseStats>()) {
			if(StatusEffect.Effects.BaseStats.Contains(stat)) {
				totalEffect.BaseStats[stat] += StatusEffect.Effects.BaseStats[stat];
			}
		}	
		for (EDerivedStats stat : TEnumRange<EDerivedStats>()) {
			if(StatusEffect.Effects.DerivedStats.Contains(stat)){
				totalEffect.DerivedStats[stat] += StatusEffect.Effects.DerivedStats[stat];
			}
		}	
	}

	return totalEffect;
}

FStatus UStatsComponent::CalculateStats()
{
	FStatus StatusEffect = CalculateEffects();
	FStatus Status = FStatus();

	for (EBaseStats stat : TEnumRange<EBaseStats>()) {
		Status.BaseStats[stat] = Status.BaseStats[stat];
		if(StatusEffect.BaseStats.Contains(stat)) {
			Status.BaseStats[stat] += StatusEffect.BaseStats[stat];
		}
	}	
	for (EDerivedStats stat : TEnumRange<EDerivedStats>()) {
		Status.DerivedStats[stat] = Status.DerivedStats[stat];
		if(StatusEffect.DerivedStats.Contains(stat)){
			Status.DerivedStats[stat] += StatusEffect.DerivedStats[stat];
		}
	}

	return Status;
}
