#include "ItemSystem.h"

#define LOCTEXT_NAMESPACE "FItemSystemModule"

void FItemSystemModule::StartupModule()
{
    UE_LOG(LogTemp, Warning, TEXT("ItemSystem module has started."))
}

void FItemSystemModule::ShutdownModule()
{
    UE_LOG(LogTemp, Warning, TEXT("ItemSystem module has shut down."))
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FItemSystemModule, ItemSystem)