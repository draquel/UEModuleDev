#include "CharacterSystems.h"

#define LOCTEXT_NAMESPACE "FCharacterSystemsModule"

void FCharacterSystemsModule::StartupModule()
{
    UE_LOG(LogTemp, Warning, TEXT("CharacterSystems module has started."))
}

void FCharacterSystemsModule::ShutdownModule()
{
    UE_LOG(LogTemp, Warning, TEXT("CharacterSystems module has shut down."))
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FCharacterSystemsModule, CharacterSystems)