#pragma once

#include "CoreMinimal.h"
#include "FDamageInfo.generated.h"

UENUM(BlueprintType, meta=(ScriptName="DamageTypeEnum"))
enum EDamageType
{
	Default UMETA(DisplayName="None"),
	Mele UMETA(DisplayName="Mele"),
	Projectile UMETA(DisplayName="Projectile"),
	Explosion UMETA(DisplayName="Explosion")
};

UENUM(BlueprintType)
enum EDamageResponse
{
	None UMETA(DisplayName="None"),
	Hit UMETA(DisplayName="Hit"),
	Stagger UMETA(DisplayName="Stagger"),
	Stun UMETA(DisplayName="Stun"),
	KockBack UMETA(DisplayName="KockBack")
};

USTRUCT(BlueprintType)
struct FDamageInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float Amount = 0;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TEnumAsByte<EDamageType> Type = EDamageType::Default;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TEnumAsByte<EDamageResponse> Response = EDamageResponse::None;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool shouldDamageInvincible = false;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool canBeBlocked = true;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool canBeParried = true;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool shouldInterupt = false;

	FDamageInfo(){}
};
