// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ModuleDevEditorTarget : TargetRules
{
	public ModuleDevEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
		ExtraModuleNames.Add("ModuleDev");
		RegisterModulesCreatedByRider();
	}

	private void RegisterModulesCreatedByRider()
	{
		ExtraModuleNames.AddRange(new string[] { "NoiseGenerator", "TerrainGenerator", "MeshGenerator", "ToolsLib", "ItemSystem", "DatabaseManager", "InteractionSystem", "CharacterSystems", "CombatSystem" });
	}
}
