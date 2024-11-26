using UnrealBuildTool;

public class TerrainGenerator : ModuleRules
{
    public TerrainGenerator(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "UnrealEd", "NavigationSystem", "ProceduralMeshComponent", "MeshGenerator", "NoiseGenerator", "ToolsLib" 
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "GeometryCore", "GeometryFramework", "DynamicMesh"
            }
        );
    }
}