#include "NoiseGenerator.h"

#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FNoiseGeneratorModule"
DEFINE_LOG_CATEGORY(NoiseGenerator);

void FNoiseGeneratorModule::StartupModule()
{
    FString PluginShaderDir = FPaths::Combine(IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*FPaths::GameSourceDir()),TEXT("NoiseGenerator/Shaders/Private")); 
    AddShaderSourceDirectoryMapping(TEXT("/NoiseShaders"), PluginShaderDir); 
    UE_LOG(NoiseGenerator, Warning, TEXT("NoiseGenerator module has started."))
}

void FNoiseGeneratorModule::ShutdownModule()
{
    UE_LOG(NoiseGenerator, Warning, TEXT("NoiseGenerator module has shut down."))
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FNoiseGeneratorModule, NoiseGenerator)