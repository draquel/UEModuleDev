#include "TerrainGenerator.h"

#define LOCTEXT_NAMESPACE "FTerrainGeneratorModule"
DEFINE_LOG_CATEGORY(TerrainGenerator);

void FTerrainGeneratorModule::StartupModule()
{
    UE_LOG(TerrainGenerator, Warning, TEXT("TerrainGenerator module has started."))
}

void FTerrainGeneratorModule::ShutdownModule()
{
    UE_LOG(TerrainGenerator, Warning, TEXT("TerrainGenerator module has shut down."))
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FTerrainGeneratorModule, TerrainGenerator)