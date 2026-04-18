using UnrealBuildTool;

public class MDFFrameworkQuickstart : ModuleRules
{
    public MDFFrameworkQuickstart(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "EnhancedInput",
                "MDFFrameworkCore",
                "MDFFrameworkProgression",
                "MDFFrameworkCombat",
                "MDFFrameworkEntity",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "GameplayTags",
                "UMG",
                "MDFFrameworkDebug"
            }
        );
    }
}