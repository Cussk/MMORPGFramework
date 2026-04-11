using UnrealBuildTool;

public class MDFFrameworkDebug : ModuleRules
{
    public MDFFrameworkDebug(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "GameplayTags",
                "MDFFrameworkCore",
                "MDFFrameworkEntity",
                "MDFFrameworkCombat",
                "MDFFrameworkProgression"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
            }
        );
    }
}