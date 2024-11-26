#include "ComputeShaders.h"

#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FComputeShadersModule"

void FComputeShadersModule::StartupModule()
{
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("ShaderLib"))->GetBaseDir(), TEXT("Shaders/ComputeShaders/Private"));
	AddShaderSourceDirectoryMapping(TEXT("/ComputeShadersShaders"), PluginShaderDir); 
}

void FComputeShadersModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FComputeShadersModule, ComputeShaders)