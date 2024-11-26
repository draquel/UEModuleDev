#include "Damage/UDamageSystemComponent.h"

#include "Stats/StatsComponent.h"

UDamageSystemComponent::UDamageSystemComponent()
{
	Stats = nullptr;	
}

float UDamageSystemComponent::Heal(float Amount)
{
	if(isDead){
		return 0;
	}
	
	if(Stats->GetBaseStat(health)+Amount > Stats->GetDerivedStat(max_health)){
		Stats->SetBaseStat(health, Stats->GetDerivedStat(max_health));
	}else{
		Stats->SetBaseStat(health, Stats->GetBaseStat(health) + Amount);
	}
	
	return Stats->GetBaseStat(health);
}

bool UDamageSystemComponent::Damage(FDamageInfo DamageInfo)
{
	if(!CanBeDamaged(DamageInfo)){
		return false;
	}

	if(DamageInfo.canBeBlocked && isBlocking)
	{
		OnBlock.Broadcast(DamageInfo.canBeParried);
		return false;
	}

	if(Stats->GetBaseStat(health) - DamageInfo.Amount > 0){
		Stats->SetBaseStat(health, Stats->GetBaseStat(health) - DamageInfo.Amount);
		if(DamageInfo.shouldInterupt || isInteruptable)	{
			OnDamage.Broadcast(DamageInfo.Response);
		}
	}else{
		Stats->SetBaseStat(health, 0);
		isDead = true;
		OnDeath.Broadcast();
	}

	return true;
}

bool UDamageSystemComponent::CanBeDamaged(FDamageInfo DamageInfo)
{
	if(!isDead && (!isInvincible || DamageInfo.shouldDamageInvincible)){
		return true;
	}
	return false;
}
