using UnrealBuildTool;

public class MDFFrameworkEntity : ModuleRules
{
    public MDFFrameworkEntity(ReadOnlyTargetRules Target) : base(Target)
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
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
            }
        );
    }
}