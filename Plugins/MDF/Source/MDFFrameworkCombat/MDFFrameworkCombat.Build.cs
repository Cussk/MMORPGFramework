using UnrealBuildTool;

public class MDFFrameworkCombat : ModuleRules
{
    public MDFFrameworkCombat(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "GameplayTags",
                "Niagara",
                "DeveloperSettings",
                "MDFFrameworkCore",
                "MDFFrameworkProgression",
                "MDFFrameworkEntity"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                
            }
        );
    }
}