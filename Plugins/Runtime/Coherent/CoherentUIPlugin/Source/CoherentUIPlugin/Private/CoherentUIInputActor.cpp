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

#include "CoherentUIInputActor.h"
#include "CoherentUIInputBindings.h"

ACoherentUIInputActor::ACoherentUIInputActor(
#if defined (COHERENT_UI_UE4_4_5_SUPPORT)
	const FPostConstructInitializeProperties& PCIP)
#else
	const FObjectInitializer& PCIP)
#endif
	: Super(PCIP)
	, InputPropagationBehaviour(ECoherentUIInputPropagationBehaviour::None)
{
}

void ACoherentUIInputActor::Initialize(ECollisionChannel CollisionChannel, TextureAddress AddressMode, EInputWidgetRaycastQuality RaycastQuality, int32 UVChannel)
{
	if (GetWorld()->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	// Setup the input sink widget
	CoherentInputForwardWidget = SNew(SCoherentInputForward).Owner(this);
	if (GEngine->IsValidLowLevel())
	{
		GEngine->GameViewport->AddViewportWidgetContent(SNew(SWeakWidget).PossiblyNullContent(CoherentInputForwardWidget.ToSharedRef()));
	}
	if (CoherentInputForwardWidget.IsValid())
	{
		CoherentInputForwardWidget->SetVisibility(EVisibility::Visible);
		CoherentInputForwardWidget->SetRaycastCollisionGroup(CollisionChannel);
		CoherentInputForwardWidget->SetRaycastTextureAddressMode(AddressMode);
		CoherentInputForwardWidget->SetRaycastQuality(RaycastQuality);
		CoherentInputForwardWidget->SetRaycastUVChannel(uint32(UVChannel));

		CoherentInputForwardWidget->OnCoherentInputWidgetMouseButtonDown.AddUObject(this, &ACoherentUIInputActor::OnCoherentInputWidgetMouseButtonDown);
		CoherentInputForwardWidget->OnCoherentInputWidgetMouseButtonUp.AddUObject(this, &ACoherentUIInputActor::OnCoherentInputWidgetMouseButtonUp);
		CoherentInputForwardWidget->OnCoherentInputWidgetKeyDown.AddUObject(this, &ACoherentUIInputActor::OnCoherentInputWidgetKeyDown);
		CoherentInputForwardWidget->OnCoherentInputWidgetKeyUp.AddUObject(this, &ACoherentUIInputActor::OnCoherentInputWidgetKeyUp);
	}
}

void ACoherentUIInputActor::SetCoherentUIInputFocus(bool FocusUI)
{
	if (!CoherentInputForwardWidget.IsValid())
	{
		UE_LOG(LogCoherentUI, Error, TEXT("Trying to set focus to Coherent UI, but the Slate widget is not available!"));
		return;
	}

	FSlateApplication& SlateApp = FSlateApplication::Get();
	if (FocusUI)
	{
		SlateApp.ResetToDefaultInputSettings();
		SlateApp.SetKeyboardFocus(CoherentInputForwardWidget);
		if (InputPropagationBehaviour == ECoherentUIInputPropagationBehaviour::Joystick ||
			InputPropagationBehaviour == ECoherentUIInputPropagationBehaviour::KeyboardAndJoystick)
		{
#if defined (COHERENT_UI_UE4_4_5_SUPPORT)
			SlateApp.SetJoystickCaptorToGameViewport();
#else
			SlateApp.SetAllUserFocusToGameViewport();
#endif
		}
	}
	else
	{
		if (IsCoherentUIFocused())
		{
			TSharedPtr<SViewport> ViewportWidget = SlateApp.GetGameViewport();
			FWidgetPath ViewportWidgetPath;
			if (SlateApp.GeneratePathToWidgetUnchecked(ViewportWidget.ToSharedRef(), ViewportWidgetPath))
			{
				// See FSceneViewport::OnKeyboardFocusReceived for conditions that need to be met to lock the mouse cursor
				APlayerController* LocalPC = GEngine->GetFirstLocalPlayerController(GetWorld());
				LocalPC->CurrentMouseCursor = EMouseCursor::None;

				FReply TheReply = FReply::Handled();

#if defined (COHERENT_UI_UE4_4_5_SUPPORT)
				TheReply.SetKeyboardFocus(ViewportWidget.ToSharedRef(), EKeyboardFocusCause::Mouse);
#else
				TheReply.SetUserFocus(ViewportWidget.ToSharedRef(), EFocusCause::Mouse);
#endif
				TheReply.UseHighPrecisionMouseMovement(ViewportWidget.ToSharedRef());
				TheReply.PreventThrottling();
				SlateApp.ProcessReply(ViewportWidgetPath, TheReply, nullptr, nullptr);
			}
			else
			{
				UE_LOG(LogCoherentUI, Error, TEXT("Trying to set focus to the game viewport, but cannot obtain the widget path for it!"));
			}
		}
	}
}

bool ACoherentUIInputActor::IsCoherentUIFocused() const
{
	return FSlateApplication::Get().GetKeyboardFocusedWidget() == CoherentInputForwardWidget;
}

void ACoherentUIInputActor::ToggleCoherentUIInputFocus()
{
	SetCoherentUIInputFocus(!IsCoherentUIFocused());
}

void ACoherentUIInputActor::SetCoherentUIViewFocus(UCoherentBaseComponent* NewFocusedView)
{
	if (CoherentInputForwardWidget.IsValid())
	{
		CoherentInputForwardWidget->RefocusView(NewFocusedView);
	}
}

void ACoherentUIInputActor::SetInputPropagationBehaviour(ECoherentUIInputPropagationBehaviour::Type Propagation)
{
	InputPropagationBehaviour = Propagation;
	if (CoherentInputForwardWidget.IsValid())
	{
		if (Propagation == ECoherentUIInputPropagationBehaviour::Keyboard ||
			Propagation == ECoherentUIInputPropagationBehaviour::KeyboardAndJoystick)
		{
			CoherentInputForwardWidget->SetPropagateKeyboardEvents(true);
		}
		else
		{
			CoherentInputForwardWidget->SetPropagateKeyboardEvents(false);
		}
	}
}

ECoherentUIInputPropagationBehaviour::Type ACoherentUIInputActor::GetInputPropagationBehaviour() const
{
	return InputPropagationBehaviour;
}

void ACoherentUIInputActor::AllowJoystickInputWhileUIIsFocused_DEPRECATED(bool bAllow)
{
	InputPropagationBehaviour = bAllow ? ECoherentUIInputPropagationBehaviour::Joystick : ECoherentUIInputPropagationBehaviour::None;
}

bool ACoherentUIInputActor::IsJoystickInputWhileUIIsFocusedAllowed_DEPRECATED() const
{
	return InputPropagationBehaviour >= ECoherentUIInputPropagationBehaviour::Joystick;
}

void ACoherentUIInputActor::AlwaysAcceptMouseInput(bool bForce)
{
	if (CoherentInputForwardWidget.IsValid())
	{
		CoherentInputForwardWidget->SetAlwaysAcceptMouseInputEvents(bForce);
	}
}

void ACoherentUIInputActor::SetLineTraceMode(EInputWidgetLineTraceMode Mode)
{
	if (CoherentInputForwardWidget.IsValid())
	{
		CoherentInputForwardWidget->SetLineTraceMode(Mode);
	}
}

static ECoherentUIKeys::Type FKeyToEnum(const FKey& Key)
{
#define COMPARE_FKEY_AND_RETURN_ENUM(FKey, CoherentVK) \
	if (EKeys::FKey == Key) return ECoherentUIKeys::FKey; \

	FKEY_MACRO_ITERATOR(COMPARE_FKEY_AND_RETURN_ENUM);

#undef COMPARE_FKEY_AND_RETURN_ENUM

	return ECoherentUIKeys::Unknown;
}

void ACoherentUIInputActor::OnCoherentInputWidgetMouseButtonDown(const FKey& Button)
{
	OnCoherentInputActorMouseButtonDown.Broadcast(FKeyToEnum(Button));
}

void ACoherentUIInputActor::OnCoherentInputWidgetMouseButtonUp(const FKey& Button)
{
	OnCoherentInputActorMouseButtonUp.Broadcast(FKeyToEnum(Button));
}

void ACoherentUIInputActor::OnCoherentInputWidgetKeyDown(const FKey& Key)
{
	OnCoherentInputActorKeyDown.Broadcast(FKeyToEnum(Key));
}

void ACoherentUIInputActor::OnCoherentInputWidgetKeyUp(const FKey& Key)
{
	OnCoherentInputActorKeyUp.Broadcast(FKeyToEnum(Key));
}

TSharedPtr<SCoherentInputForward> ACoherentUIInputActor::GetWidget()
{
	return CoherentInputForwardWidget;
}
