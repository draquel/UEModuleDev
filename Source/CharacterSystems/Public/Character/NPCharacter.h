// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Interactable.h"
#include "Perception/PawnSensingComponent.h"
#include "NPCharacter.generated.h"

UCLASS()
class CHARACTERSYSTEMS_API ANPCharacter : public ABaseCharacter, public IInteractable
{
	GENERATED_BODY()

public:
	ANPCharacter();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	UPawnSensingComponent* PawnSensing;	

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Death();

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual bool CanInteract(AActor* Interactor) const override;	
};
