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

#include "../CoherentUIPluginPrivatePCH.h"

#include "Menu/CoherentUIMenu.h"

#include "Coherent/UI/View.h"
#include "Coherent/UI/Binding/EventHandler.h"

#include "CoherentFStringBinder.h"
#include "CoherentTArrayBinder.h"
#include "CoherentFVectorBinder.h"

TSet<uint32> UCoherentUIMenu::IsMenuReady;

UCoherentUIMenu::UCoherentUIMenu(
#if defined (COHERENT_UI_UE4_4_5_SUPPORT)
	const FPostConstructInitializeProperties& PCIP)
#else
	const FObjectInitializer& PCIP)
#endif
	: Super(PCIP)
	, MenuViewComponent(nullptr)
{
}

void UCoherentUIMenu::Initialize(UCoherentBaseComponent* ViewComponent, FCoherentUIMenuInfo& InMenuInfo)
{
	MenuViewComponent = ViewComponent;
	MenuInfo = InMenuInfo;

	auto View = MenuViewComponent->GetView();
	if (View)
	{
		if (MenuViewComponent->IsReadyForBindings())
		{
			SetupScripting(View);
		}
		else
		{
			MenuViewComponent->ReadyForBindings.AddDynamic(this, &UCoherentUIMenu::OnReadyForScripting);
		}
		MenuViewComponent->BindingsReleased.AddDynamic(this, &UCoherentUIMenu::OnBindingsReleased);
	}
}

void UCoherentUIMenu::Show()
{
	if (MenuViewComponent.IsValid())
	{
		auto View = MenuViewComponent->GetView();
		if (View)
		{
			if (IsMenuReady.Contains(View->GetId()))
			{
				View->TriggerEvent("cui.ShowMenu", MenuInfo.MenuID);
				MenuInfo.bVisible = true;
			}
			else
			{
				MenuReady.AddDynamic(this, &UCoherentUIMenu::ShowMenuWhenReady);
			}
		}
	}
}

void UCoherentUIMenu::Hide()
{
	if (MenuViewComponent.IsValid())
	{
		auto View = MenuViewComponent->GetView();
		if (View)
		{
			if (IsMenuReady.Contains(View->GetId()))
			{
				View->TriggerEvent("cui.HideMenu", MenuInfo.MenuID);
				MenuInfo.bVisible = false;
			}
			else
			{
				MenuReady.AddDynamic(this, &UCoherentUIMenu::HideMenuWhenReady);
			}
		}
	}
}

void UCoherentUIMenu::OnMenuButtonClicked(const FString& MenuID, const FString& ButtonID)
{
	if (MenuInfo.MenuID == MenuID)
	{
		MenuButtonClicked.Broadcast(ButtonID);
	}
}

void UCoherentUIMenu::SetupScripting(Coherent::UI::View* View)
{
	View->RegisterForEvent("cui.MenuButtonClicked", Coherent::UI::MakeHandler(this,
		&UCoherentUIMenu::OnMenuButtonClicked));
	View->RegisterForEvent("cui.MenuReady", Coherent::UI::MakeHandler(this,
		&UCoherentUIMenu::OnMenuReady));
}


void CoherentBind(Coherent::UI::Binder* Binder, FCoherentUIMenuInfo* Info)
{
	if (auto type = Binder->RegisterType("CUIMenuInfo", Info))
	{
		type.Property("Id", &FCoherentUIMenuInfo::MenuID)
			.Property("Parent", &FCoherentUIMenuInfo::ParentElementID)
			.Property("Visible", &FCoherentUIMenuInfo::bVisible)
			.Property("Buttons", &FCoherentUIMenuInfo::Buttons);
	}
}

void CoherentBind(Coherent::UI::Binder* Binder, FCoherentUIMenuButton* Button)
{
	if (auto type = Binder->RegisterType("CUIMenuButton", Button))
	{
		type.Property("Label", &FCoherentUIMenuButton::Label)
			.Property("Disabled", &FCoherentUIMenuButton::bDisabled);
	}
}

void UCoherentUIMenu::OnMenuReady()
{
	auto View = MenuViewComponent->GetView();
	IsMenuReady.Add(View->GetId(), nullptr);
	View->TriggerEvent("cui.CreateMenu", MenuInfo.MenuID, MenuInfo);

	MenuReady.Broadcast();
}

void UCoherentUIMenu::OnReadyForScripting(int32, const FString&, bool IsMainFrame)
{
	if (IsMainFrame)
	{
		if (MenuViewComponent.IsValid())
		{
			auto View = MenuViewComponent->GetView();
			if (View)
			{
				SetupScripting(View);
			}
		}
	}
}

void UCoherentUIMenu::OnBindingsReleased(int32, const FString&, bool IsMainFrame)
{
	if (IsMainFrame)
	{
		auto View = MenuViewComponent->GetView();
		if (View)
		{
			IsMenuReady.Remove(View->GetId());
		}
	}
}

void UCoherentUIMenu::ShowMenuWhenReady()
{
	if (MenuViewComponent.IsValid())
	{
		auto View = MenuViewComponent->GetView();
		if (View)
		{
			View->TriggerEvent("cui.ShowMenu", MenuInfo.MenuID);
			MenuInfo.bVisible = true;
		}
	}
}

void UCoherentUIMenu::HideMenuWhenReady()
{
	if (MenuViewComponent.IsValid())
	{
		auto View = MenuViewComponent->GetView();
		if (View)
		{
			View->TriggerEvent("cui.HideMenu", MenuInfo.MenuID);
			MenuInfo.bVisible = false;
		}
	}
}
