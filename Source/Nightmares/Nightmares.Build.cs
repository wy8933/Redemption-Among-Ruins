using System;
using System.IO;
using UnrealBuildTool;

public class Nightmares : ModuleRules
{
	public Nightmares(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "InputCore" });

		Console.WriteLine(Path.Combine(ModuleDirectory, "alice-network/include"));
		
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "alice-network/include"));
	}
}