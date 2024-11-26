using UnrealBuildTool;

public class ComputeShaders : ModuleRules
{
    public ComputeShaders(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PrivateIncludePaths.AddRange(new string[] 
        {
            "Runtime/Renderer/Private",
            "ComputeShaders/Private"
        });
        
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "Engine",
                "MaterialShaderQualitySettings"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Renderer",
                "RenderCore",
                "RHI",
                "Projects"
            }
        );
        if (Target.bBuildEditor == true) {
            PrivateDependencyModuleNames.AddRange(new string[] {
                    "UnrealEd",
                    "MaterialUtilities",
                    "SlateCore",
                    "Slate",
                    "TargetPlatform"
                }
            );
        }
        
        CircularlyReferencedDependentModules.AddRange(
            new string[] {
                "UnrealEd",
                "MaterialUtilities",
            }
        );
    }
}