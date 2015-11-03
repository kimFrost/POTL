/*
This file is part of Coherent UI, modern user interface library for
games.

Copyright (c) 2012-2014 Coherent Labs AD and/or its licensors. All
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

#include "CoherentUIHUD.h"
#include "Coherent/UI/ViewContext.h"
#include "Coherent/UI/View.h"
#include "Coherent/UI/ViewInfo.h"

UCoherentUIHUD::UCoherentUIHUD(
#if defined (COHERENT_UI_UE4_4_5_SUPPORT)
	const FPostConstructInitializeProperties& PCIP)
#else
	const FObjectInitializer& PCIP)
#endif
	: Super(PCIP)
	, bIsOnDemand(false)
	, bHasRequestedView(false)
{
	bWantsInitializeComponent = true;
}

void UCoherentUIHUD::InitializeComponent()
{
	Super::InitializeComponent();
	CreateSystemIfNecessary(GetWorld());
}

void UCoherentUIHUD::DrawHUD(class UCanvas* Canvas, int32 positionX, int32 positionY)
{
	auto texture = GetTexture();
	auto view = GetView();
	if (view && texture && texture->Resource)
	{
		FCanvasTileItem Tile(FVector2D(positionX, positionY),
			texture->Resource,
			FLinearColor::White);
		Tile.BlendMode = SE_BLEND_AlphaComposite;
		Canvas->DrawItem(Tile);
	}
}

void UCoherentUIHUD::RegisterComponentTickFunctions(bool bRegister)
{
	Super::RegisterComponentTickFunctions(bRegister);

	RegisterTickFunctions(bRegister);
}

void UCoherentUIHUD::CreateHUDView(
	const Coherent::UI::ViewInfo& info,
	const FString& url)
{
	if (!IsReadyToCreateView())
		return;

	auto sysActor = GetSystemActor(GetWorld());
	if (!sysActor)
		return;

	System = sysActor->GetSystemObject();

	System->CreateView(this, info, *url);

	bHasRequestedView = true;

	if (info.IsOnDemand)
	{
		PostPhysicsComponentTick.bTickEvenWhenPaused = sysActor->UpdateWhenPaused;
	}
	else
	{
		PrimaryComponentTick.bTickEvenWhenPaused = sysActor->UpdateWhenPaused;
	}

	bIsOnDemand = info.IsOnDemand;
}

bool UCoherentUIHUD::HasRequestedView() const
{
	return bHasRequestedView;
}

void UCoherentUIHUD::TickCustom(FCoherentComponentTickFunction* function)
{
	if (function == &PostPhysicsComponentTick)
	{
		TickListener();
	}

	if (!bIsOnDemand || !GetView())
		return;

	if (function == &PostPhysicsComponentTick)
	{
		GetView()->RequestFrame();
		ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
			CoherentFetchSurface,
			TWeakObjectPtr<UCoherentBaseComponent>, CoherentComponent, this,
			{
				if (CoherentComponent.IsValid())
				{
					Coherent::UI::View* View = CoherentComponent->GetView();
					if (View)
					{
						View->FetchSurface();
					}
				}
			}
		);
	}
}
