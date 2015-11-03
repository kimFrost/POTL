/*
This file is part of Coherent UI, modern user interface library for
games.

Copyright (c) 2012-2014 Coherent Labs Limited and/or its licensors. All
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

#include "Coherent/UI/View.h"
#include "Coherent/UI/Binding/EventHandler.h"

#include "CoherentTArrayBinder.h"
#include "CoherentFStringBinder.h"
#include "Menu/CoherentUIOptionsMenu.h"

void CoherentBind(Coherent::UI::Binder* Binder, FCoherentUISavedOptions* SavedOptions)
{
	if (auto type = Binder->RegisterType("FCoherentUISavedOptions", SavedOptions))
	{
		type.Property("Name", &FCoherentUISavedOptions::Name)
			.Property("ControlType", &FCoherentUISavedOptions::Type)
			.Property("CurrentValue", &FCoherentUISavedOptions::CurrentValue);
	}
}

UCoherentUIOptionsMenu::UCoherentUIOptionsMenu(
#if defined (COHERENT_UI_UE4_4_5_SUPPORT)
	const FPostConstructInitializeProperties& PCIP)
#else
	const FObjectInitializer& PCIP)
#endif
	: Super(PCIP)
	, OptionsMenuViewComponent(nullptr)
{
}

void UCoherentUIOptionsMenu::Initialize(UCoherentBaseComponent* ViewComponent, TArray<FCoherentUIOptionsMenuControl> controls)
{
	OptionsMenuViewComponent = ViewComponent;
	Controls = controls;

	auto View = OptionsMenuViewComponent->GetView();
	if (View)
	{
		if (OptionsMenuViewComponent->IsReadyForBindings())
		{
			SetupScripting(View);
		}
		else
		{
			OptionsMenuViewComponent->ReadyForBindings.AddDynamic(this, &UCoherentUIOptionsMenu::OnReadyForScripting);
		}
	}
}

void UCoherentUIOptionsMenu::OnReadyForScripting(int32, const FString&, bool IsMainFrame)
{
	if (IsMainFrame)
	{
		if (OptionsMenuViewComponent.IsValid())
		{
			auto View = OptionsMenuViewComponent->GetView();
			if (View)
			{
				SetupScripting(View);
			}
		}
	}
}

void UCoherentUIOptionsMenu::SetupScripting(Coherent::UI::View* View)
{
	View->BindCall("optionsSaved", Coherent::UI::MakeHandler(this,
		&UCoherentUIOptionsMenu::OnOptionsSaved));
	View->RegisterForEvent("optionsDiscarded", Coherent::UI::MakeHandler(this,
		&UCoherentUIOptionsMenu::OnOptionsDiscarded));

	TArray<TArray<FString>> JSValues;
	TArray<TArray<int32>> SliderValues;
	TArray<TArray<FString>> DropdownOptions;

	for (int i = 0; i < Controls.Num(); ++i)
	{
		JSValues.Add(TArray<FString>());
		SliderValues.Add(TArray<int32>());
		DropdownOptions.Add(TArray<FString>());

		auto control = Controls[i];
		JSValues[i].Add(control.Name);
		JSValues[i].Add(FString::FromInt(control.ID));
		JSValues[i].Add(FString::FromInt(control.Type));
		JSValues[i].Add(control.DefaultValue);
		JSValues[i].Add(FString::FromInt(control.MinNum));
		JSValues[i].Add(FString::FromInt(control.MaxNum));

		for (int j = 0; j < control.SliderValues.Num(); ++j)
		{
			SliderValues[i].Add(control.SliderValues[j]);
		}

		for (int j = 0; j < control.DropdownOptions.Num(); ++j)
		{
			DropdownOptions[i].Add(control.DropdownOptions[j]);
		}
	}

	View->TriggerEvent("setupOptionsMenu", JSValues, SliderValues, DropdownOptions);
}



void UCoherentUIOptionsMenu::OnOptionsSaved(TArray<FCoherentUISavedOptions> savedOptions)
{
	OnMenuOptionsSaved.Broadcast(savedOptions);
}

void UCoherentUIOptionsMenu::OnOptionsDiscarded()
{
	OnMenuOptionsDiscarded.Broadcast();
}