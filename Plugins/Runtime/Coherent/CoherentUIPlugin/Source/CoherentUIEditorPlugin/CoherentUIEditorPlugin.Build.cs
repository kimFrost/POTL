// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class CoherentUIEditorPlugin : ModuleRules
	{
		public CoherentUIEditorPlugin(TargetInfo Target)
		{
			string CoherentUIDir = UEBuildConfiguration.UEThirdPartySourceDirectory +
				"CoherentUI/";

			string CoherentUIIncludeDir = CoherentUIDir + "include/";
			PublicIncludePaths.AddRange(
				new string[] {
					// ... add public include paths required here ...
					CoherentUIIncludeDir
				}
				);

			PrivateIncludePaths.AddRange(
				new string[] {
					"Runtime/CoherentUIEditorPlugin/Private",
				}
				);

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"UnrealEd",
					"Core",
					"CoreUObject",
					"Engine",
					"EngineSettings",
					"EditorStyle",
					"Slate",
					"SlateCore",
					"LevelEditor",
					"AssetTools",
					"Kismet",
					"KismetCompiler",
				}
				);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"CoherentUIEditorPlugin",
					"CoherentUIPlugin",
				}
				);

			DynamicallyLoadedModuleNames.AddRange(
				new string[]
				{
				}
				);
		}
	}
}
