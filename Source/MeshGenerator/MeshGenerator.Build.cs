using UnrealBuildTool;

public class MeshGenerator : ModuleRules
{
    public MeshGenerator(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        // PrivateIncludePaths.AddRange(new string[] 
        // {
        //     "Runtime/Renderer/Private",
        //     "ComputeShaders/Private"
        // });
        
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "NoiseGenerator", "ToolsLib", "ProceduralMeshComponent", "GeometryCore", "GeometryFramework", "DynamicMesh",
                "MaterialShaderQualitySettings"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
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