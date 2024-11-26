using UnrealBuildTool;

public class CharacterSystems : ModuleRules
{
    public CharacterSystems(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "ItemSystem", "EnhancedInput", "AIModule"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore", "InteractionSystem", "CombatSystem", "ToolsLib"
            }
        );
    }
}