// Copyright (c) 2023 HTC Corporation. All Rights Reserved.

using UnrealBuildTool;

public class ViveOpenXRHandInteraction : ModuleRules
{
	public ViveOpenXRHandInteraction(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "OpenXRHMD",
                "InputCore"
				// ... add private dependencies that you statically link with here ...	
			}
			);

        AddEngineThirdPartyPrivateStaticDependencies(Target, "OpenXR");
        DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
