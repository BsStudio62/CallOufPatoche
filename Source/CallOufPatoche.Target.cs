// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class CallOufPatocheTarget : TargetRules
{
	public CallOufPatocheTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

        // Update build settings
        DefaultBuildSettings = BuildSettingsVersion.Latest;

        // Update include order version
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

        ExtraModuleNames.AddRange( new string[] { "CallOufPatoche" } );
	}
}
