/*
This file is part of Coherent UI, modern user interface library for
games.

Copyright (c) 2012-2015 Coherent Labs AD and/or its licensors. All
rights reserved in all media.

The coded instructions, statements, computer programs, and/or related
material (collectively the "Data") in these files contain confidential
and unpublished information proprietary Coherent Labs and/or its
licensors, which is protected by United States of America federal
copyright law and by international treaties.

This software or source code is supplied under the terms of a license
agreement and nondisclosure agreement with Coherent Labs Limited and may
not be copied, disclosed, or exploited except in accordance with the
terms of that agreement. The Data may not be disclosed or distributed to
third parties, in whole or in part, without the prior written consent of
Coherent Labs Limited.

COHERENT LABS MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS
SOURCE CODE FOR ANY PURPOSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER, ITS AFFILIATES,
PARENT COMPANIES, LICENSORS, SUPPLIERS, OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OR PERFORMANCE OF THIS SOFTWARE OR SOURCE CODE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "CoherentUIPluginPrivatePCH.h"
#include "CoherentUIGameHUD.h"


#include "Coherent/UI/ViewInfo.h"
#include "Coherent/UI/View.h"

#include "Coherent/UI/Binding/EventHandler.h"
#include "CoherentFStringBinder.h"

ACoherentUIGameHUD::ACoherentUIGameHUD(
#if defined (COHERENT_UI_UE4_4_5_SUPPORT)
	const FPostConstructInitializeProperties& PCIP)
#else
	const FObjectInitializer& PCIP)
#endif
	: Super(PCIP)
{
	CoherentUIHUD = CoherentUIGetSubobjectPtr(PCIP.CreateDefaultSubobject<UCoherentUIHUD>(this, TEXT("CoherentUIHUD")));

	Info.Width = 1;
	Info.Height = 1;
	Info.UsesSharedMemory = false;
	Info.IsTransparent = true;
	Info.SupportClickThrough = true;
	URL = "coui://uiresources/hud.html";
}

void ACoherentUIGameHUD::DrawHUD()
{
	Super::DrawHUD();

	if (CoherentUIHUD)
	{
		if (CoherentUIHUD->IsReadyToCreateView() && !CoherentUIHUD->HasRequestedView())
		{
			CoherentUIHUD->CreateHUDView(Info, URL);
		}
		// check that the size mathes
		auto view = CoherentUIHUD->GetView();
		if (view)
		{
			if (view->GetWidth() != Canvas->ClipX
				|| view->GetHeight() != Canvas->ClipY)
			{
				CoherentUIHUD->Resize(Canvas->ClipX, Canvas->ClipY);
			}
		}
		CoherentUIHUD->DrawHUD(Canvas, 0, 0);
	}
}

UCoherentUIHUD* ACoherentUIGameHUD::GetCoherentUIHUD() const
{
	return CoherentUIHUD;
}

void ACoherentUIGameHUD::SetupUIView(const FString& PageUrl,
									 bool bUsesSharedMemory,
									 bool bSupportClickThrough,
									 float ClickThroughAlphaThreshold,
									 bool bIsTransparent,
									 int32 TargetFrameRate,
									 bool bForceSoftwareRendering)
{
	Info.UsesSharedMemory = bUsesSharedMemory;
	Info.SupportClickThrough = bSupportClickThrough;
	Info.ClickThroughAlphaThreshold = ClickThroughAlphaThreshold;
	Info.IsTransparent = bIsTransparent;
	Info.TargetFrameRate = TargetFrameRate;
	Info.ForceSoftwareRendering = bForceSoftwareRendering;

	URL = PageUrl;
}