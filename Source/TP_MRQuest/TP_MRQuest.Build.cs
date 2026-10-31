// Szablon MR Quest 3 – moduł główny.
// Przy tworzeniu projektu z szablonu nazwa modułu zostanie zamieniona na nazwę projektu.

using UnrealBuildTool;

public class TP_MRQuest : ModuleRules
{
	public TP_MRQuest(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",

			// XR (Motion Controller, HMD)
			"HeadMountedDisplay",
			"XRBase"
		});

		// Nazwy sprawdzone w Engine/Plugins/Marketplace/MetaXR i MetaXRInteraction/Source.
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"OculusInteraction",          // ISDK: grabbable, poke, ray
			"OculusInteractionPrebuilts", // ISDK: rigi i wizualizacja dłoni
			"OculusXRHMD",                // EOculusXRPassthroughLayerOrder, kształty warstw passthrough
			"OculusXRPassthrough"         // UOculusXRPassthroughSubsystem (persistent passthrough)
		});

		// Odkomentuj, gdy zaczniesz używać tych funkcji z C++.
		// PrivateDependencyModuleNames.AddRange(new string[]
		// {
		// 	"OculusXRAnchors",     // anchory przestrzenne
		// 	"OculusXRScene",       // scena / pokój
		// 	"MRUtilityKit"         // Meta MR Utility Kit (wymaga włączonego pluginu MRUK)
		// });
	}
}
