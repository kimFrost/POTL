#include "CoherentUIPluginPrivatePCH.h"
#include "CoherentUISettings.h"


UCoherentUISettings::UCoherentUISettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, DebuggerPort(9999)
{
	FString ResourcesDir = FPaths::Combine(*FPaths::EnginePluginsDir(),
		ANSI_TO_TCHAR("Runtime"),
		ANSI_TO_TCHAR("Coherent"),
		ANSI_TO_TCHAR("CoherentUIPlugin"),
		ANSI_TO_TCHAR("Resources"));
	DebuggerPath = FPaths::Combine(*ResourcesDir,
		ANSI_TO_TCHAR("Debugger"),
		ANSI_TO_TCHAR("Debugger.exe"));
	DebuggerPath = FPaths::ConvertRelativePathToFull(DebuggerPath);
}
