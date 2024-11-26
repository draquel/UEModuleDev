#pragma once

#include "CoreMinimal.h"
#include "FStatus.generated.h"

UENUM(BlueprintType)
enum EBaseStats 
{
	level,
	health,
	stamina,
	magic,
	strength,
	constitution,
	dexterity,
	intelligence,
	wisdom,
	charisma,
	BCOUNT UMETA(Hidden)
};

UENUM(BlueprintType)
enum EDerivedStats
{
	max_health,
	max_stamina,
	max_magic,
	attack_power,
	attack_speed,
	spell_power,
	movement_speed,
	DCOUNT UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(EBaseStats, EBaseStats::BCOUNT)
ENUM_RANGE_BY_COUNT(EDerivedStats, EDerivedStats::DCOUNT);

USTRUCT(BlueprintType)
struct FStatus{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TEnumAsByte<EBaseStats>,float> BaseStats;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TEnumAsByte<EDerivedStats>,float> DerivedStats;
	
	void InitializeAll()
	{
		for (EBaseStats stat : TEnumRange<EBaseStats>()) { BaseStats.Add(stat,1); }
		for (EDerivedStats stat : TEnumRange<EDerivedStats>()) { DerivedStats.Add(stat,1); }
	}

	void CopyFrom(const FStatus& other)
	{
		for (EBaseStats stat : TEnumRange<EBaseStats>()) { if(other.BaseStats.Contains(stat)){ BaseStats[stat] = other.BaseStats[stat]; } }
		for (EDerivedStats stat : TEnumRange<EDerivedStats>()) { if(other.DerivedStats.Contains(stat)){ DerivedStats[stat] = other.DerivedStats[stat]; } }	
	}
};

USTRUCT(BlueprintType)
struct FStatusEffect
{
	GENERATED_BODY()
	
	FName Name;
	FStatus Effects;
};