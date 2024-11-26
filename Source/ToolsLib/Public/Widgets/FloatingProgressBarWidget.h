// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FloatingProgressBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class TOOLSLIB_API UFloatingProgressBarWidget : public UUserWidget
{
	GENERATED_BODY()

	public:
		// Updates the progress bar with a new health value (between 0.0 and 1.0)
		UFUNCTION(BlueprintCallable, Category = "UI")
		void UpdateHealthBar(float HealthPercentage);

	protected:
		// Override the native construct function to initialize the widget
		virtual void NativeConstruct() override;

		// Reference to a progress bar widget in the UI
		UPROPERTY(meta = (BindWidget))
		class UProgressBar* ProgressBar;
};
