#include "../CoherentUIPluginPrivatePCH.h"

#include "Menu/CoherentUIOptionsMenuHelper.h"

UCoherentUIOptionsMenuHelper::UCoherentUIOptionsMenuHelper(
#if defined (COHERENT_UI_UE4_4_5_SUPPORT)
	const FPostConstructInitializeProperties& PCIP)
#else
	const FObjectInitializer& PCIP)
#endif
	: Super(PCIP)
{
}