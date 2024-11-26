
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

UINTERFACE(BlueprintType)
class INTERACTIONSYSTEM_API UInteractable : public UInterface
{
	GENERATED_BODY()
};


class INTERACTIONSYSTEM_API IInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Interact(AActor* Interactor);

	// Optional: Check if the object is interactable by a specific actor
	virtual bool CanInteract(AActor* Interactor) const = 0;
};
