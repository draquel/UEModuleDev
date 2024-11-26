#include "DatabaseManager.h"

#define LOCTEXT_NAMESPACE "FDatabaseManagerModule"

void FDatabaseManagerModule::StartupModule()
{
    UE_LOG(LogTemp, Warning, TEXT("DatabaseManager module has started."))
}

void FDatabaseManagerModule::ShutdownModule()
{
    UE_LOG(LogTemp, Warning, TEXT("DatabaseManager module has shut down."))
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FDatabaseManagerModule, DatabaseManager)