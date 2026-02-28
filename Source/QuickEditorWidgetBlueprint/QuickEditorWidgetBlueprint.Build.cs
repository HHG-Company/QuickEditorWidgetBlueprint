using UnrealBuildTool;

public class QuickEditorWidgetBlueprint : ModuleRules
{
	public QuickEditorWidgetBlueprint(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"UMG"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Slate",
			"SlateCore",
			"UnrealEd",
			"Blutility",
			"UMGEditor",
			"EditorSubsystem",
			"ScriptableEditorWidgets"
		});
	}
}