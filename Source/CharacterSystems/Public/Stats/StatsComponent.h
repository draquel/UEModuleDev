#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FStatus.h"
#include "StatsComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CHARACTERSYSTEMS_API UStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStatsComponent();

	UFUNCTION(BlueprintCallable)
	float GetBaseStat(EBaseStats stat);
	UFUNCTION(BlueprintCallable)
	float GetDerivedStat(EDerivedStats stat);

	UFUNCTION(BlueprintCallable)
	void SetBaseStat(EBaseStats stat, float value);
	UFUNCTION(BlueprintCallable)
	void SetDerivedStat(EDerivedStats stat, float value);

	UFUNCTION(BlueprintCallable)
	void UpdateDerivedStats();
	
	UFUNCTION(BlueprintCallable)
	FStatus GetStatus();
	
	FStatus CalculateEffects();
	UFUNCTION(BlueprintCallable)
	FStatus CalculateStats();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FStatus Defaults;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FStatus Stats;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FStatusEffect> Effects;
	
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
