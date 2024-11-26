// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/FloatingProgressBarWidget.h"

#include "Components/ProgressBar.h"

void UFloatingProgressBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Initialize default state of the health bar, for example, at 100% health
	if (ProgressBar) {
		ProgressBar->SetPercent(1.0f); // Full health (100%)
	}
}

void UFloatingProgressBarWidget::UpdateHealthBar(float HealthPercentage)
{
	if (ProgressBar) {
		ProgressBar->SetPercent(HealthPercentage);
	}
}