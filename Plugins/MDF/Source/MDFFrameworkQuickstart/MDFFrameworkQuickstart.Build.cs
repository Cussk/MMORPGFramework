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
                "GameplayTags",
                "MDFFrameworkCore",
                "MDFFrameworkEntity",
                "MDFFrameworkCombat"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                
            }
        );
    }
}