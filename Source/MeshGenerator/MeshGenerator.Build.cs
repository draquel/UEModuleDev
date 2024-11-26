using UnrealBuildTool;

public class MeshGenerator : ModuleRules
{
    public MeshGenerator(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "NoiseGenerator", "ToolsLib", "ProceduralMeshComponent", "GeometryCore", "GeometryFramework", "DynamicMesh"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore"
            }
        );
    }
}