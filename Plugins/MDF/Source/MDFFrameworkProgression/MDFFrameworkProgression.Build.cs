using UnrealBuildTool;

public class MDFFrameworkProgression : ModuleRules
{
    public MDFFrameworkProgression(ReadOnlyTargetRules Target) : base(Target)
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
                "MDFFrameworkCore"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
            }
        );
    }
}