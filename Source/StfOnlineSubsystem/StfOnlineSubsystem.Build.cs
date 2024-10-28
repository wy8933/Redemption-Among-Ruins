using UnrealBuildTool;

public class StfOnlineSubsystem : ModuleRules
{
	public StfOnlineSubsystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core", "CoreUObject", "Engine", "InputCore", "OnlineSubsystem", "OnlineSubsystemUtils"
			}
		);
	}
}