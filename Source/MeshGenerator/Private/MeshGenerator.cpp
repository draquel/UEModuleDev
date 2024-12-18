#include "MeshGenerator.h"

#define LOCTEXT_NAMESPACE "FMeshGeneratorModule"
DEFINE_LOG_CATEGORY(MeshGenerator);

void FMeshGeneratorModule::StartupModule()
{
    FString PluginShaderDir = FPaths::Combine(IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*FPaths::GameSourceDir()),TEXT("MeshGenerator/Shaders/Private")); 
    AddShaderSourceDirectoryMapping(TEXT("/MeshShaders"), PluginShaderDir); 
    UE_LOG(LogTemp, Warning, TEXT("MeshGenerator module has started."))
}

void FMeshGeneratorModule::ShutdownModule()
{
    UE_LOG(LogTemp, Warning, TEXT("MeshGenerator module has shut down."))
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FMeshGeneratorModule, MeshGenerator)