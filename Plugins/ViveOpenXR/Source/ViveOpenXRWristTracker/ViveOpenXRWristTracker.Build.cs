// Copyright HTC Corporation. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

namespace UnrealBuildTool.Rules
{
    public class ViveOpenXRWristTracker : ModuleRules
    {
        public ViveOpenXRWristTracker(ReadOnlyTargetRules Target) 
				: base(Target)
        {
			var EngineDir = Path.GetFullPath(Target.RelativeEnginePath);
            PrivateIncludePaths.AddRange(
                new string[] {
                    EngineDir + "/Source/ThirdParty/OpenXR/include"
				}
                );

            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                    "CoreUObject",
					"ApplicationCore",
                    "Engine",
                    "InputDevice",
                    "InputCore",
                    "HeadMountedDisplay",
                    "UMG",
                    "OpenXRHMD",
					"OpenXRInput"
				}
				);

            AddEngineThirdPartyPrivateStaticDependencies(Target, "OpenXR");

            if (Target.bBuildEditor == true)
            {
                PrivateDependencyModuleNames.Add("UnrealEd");
            }
        }
    }
}
