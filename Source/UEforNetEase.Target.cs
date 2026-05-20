using UnrealBuildTool;
using System.Collections.Generic;

public class UEforNetEaseTarget : TargetRules
{
	public UEforNetEaseTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("PoisonCircle");
	}
}
