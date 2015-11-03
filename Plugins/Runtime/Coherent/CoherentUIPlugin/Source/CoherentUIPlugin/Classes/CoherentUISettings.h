#pragma once

#include "CoherentUISettings.generated.h"

/**
 * Implements the settings for the Coherent UI plugin.
 */
UCLASS(config = Engine, defaultconfig)
class COHERENTUIPLUGIN_API UCoherentUISettings : public UObject
{
	GENERATED_UCLASS_BODY()

	//  The port used for the Coherent UI Debugger -  set to -1 to disable debugging
	UPROPERTY(Config, EditAnywhere, Category = Settings)
	int32 DebuggerPort;

	// The path to Coherent UI's Debugger
	UPROPERTY(Config, EditAnywhere, Category = Settings)
	FString DebuggerPath;
};
