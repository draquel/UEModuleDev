// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ModuleDevTarget : TargetRules
{
	public ModuleDevTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		ExtraModuleNames.Add("ModuleDev");
		RegisterModulesCreatedByRider();
	}

	private void RegisterModulesCreatedByRider()
	{
		ExtraModuleNames.AddRange(new string[] { "NoiseGenerator", "TerrainGenerator", "MeshGenerator", "ToolsLib", "ItemSystem", "DatabaseManager", "InteractionSystem", "CharacterSystems", "CombatSystem" });
	}
}
