#pragma once
#include "CoreMinimal.h"
#include "FDamageInfo.h"
#include "UDamageSystemComponent.generated.h"

class UStatsComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDeathDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDamageDelegate,EDamageResponse,Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBlockDelegate,bool,canBeParried);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CHARACTERSYSTEMS_API UDamageSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDamageSystemComponent();

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UStatsComponent* Stats;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool isInvincible = false;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool isDead = false;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool isBlocking = false;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool isInteruptable = true;

	UFUNCTION(BlueprintCallable)
	float Heal(float Amount);
	UFUNCTION(BlueprintCallable)
	bool Damage(FDamageInfo DamageInfo);
	UFUNCTION(BlueprintCallable)
	bool CanBeDamaged(FDamageInfo DamageInfo);

	UPROPERTY(BlueprintAssignable)
	FDeathDelegate OnDeath;

	UPROPERTY(BlueprintAssignable)
	FBlockDelegate OnBlock;

	UPROPERTY(BlueprintAssignable)
	FDamageDelegate OnDamage;
};
