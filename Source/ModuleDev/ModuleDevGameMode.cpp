// Copyright Epic Games, Inc. All Rights Reserved.

#include "ModuleDevGameMode.h"
#include "ModuleDevCharacter.h"
#include "UObject/ConstructorHelpers.h"

AModuleDevGameMode::AModuleDevGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
