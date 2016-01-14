// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PuzzlePreset : ModuleRules
{
    public PuzzlePreset(TargetInfo Target)
    {
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

        PublicDependencyModuleNames.AddRange(new string[] {
             "OnlineSubsystem",
             "OnlineSubsystemUtils"
         });
        DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
        DynamicallyLoadedModuleNames.Add("OnlineSubsystemNull"); // remove
    }
}
