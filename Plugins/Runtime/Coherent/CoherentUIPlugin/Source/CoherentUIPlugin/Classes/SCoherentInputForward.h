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
#pragma once

#include "Coherent/UI/InputEvents.h"

#include "CoherentUIBackwardsCompatibility.h"

#if defined (COHERENT_UI_UE4_4_5_SUPPORT)
#include "Slate.h"
#else
#include "SlateBasics.h"
#endif

#include "SCompoundWidget.h"
#include "SCoherentInputForward.generated.h"

namespace Coherent
{
	namespace UI
	{
		class View;
	}
}

class UCoherentBaseComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FCoherentInputWidgetMouseButtonDown, const FKey&);
DECLARE_MULTICAST_DELEGATE_OneParam(FCoherentInputWidgetMouseButtonUp, const FKey&);
DECLARE_MULTICAST_DELEGATE_OneParam(FCoherentInputWidgetKeyDown, const FKey&);
DECLARE_MULTICAST_DELEGATE_OneParam(FCoherentInputWidgetKeyUp, const FKey&);

UENUM()
enum EInputWidgetRaycastQuality
{
	RQ_Fast UMETA(DisplayName="Fast"),
	RQ_Balanced UMETA(DisplayName="Balanced"),
	RQ_Accurate UMETA(DisplayName="Accurate"),
	RQ_MAX,
};

UENUM()
enum EInputWidgetLineTraceMode
{
	InputLineTrace_Single UMETA(DisplayName = "Single object trace"),
	InputLineTrace_Multi UMETA(DisplayName = "Multiple objects trace"),
	InputLineTrace_MAX
};

class SCoherentInputForward : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SCoherentInputForward) {}
		SLATE_ARGUMENT(TWeakObjectPtr<AActor>, Owner)
	SLATE_END_ARGS()

	COHERENTUIPLUGIN_API virtual ~SCoherentInputForward();
	COHERENTUIPLUGIN_API void Construct(const FArguments& InArgs);

	COHERENTUIPLUGIN_API virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	COHERENTUIPLUGIN_API virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	COHERENTUIPLUGIN_API virtual FReply OnMouseButtonDoubleClick( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	COHERENTUIPLUGIN_API virtual FReply OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	COHERENTUIPLUGIN_API virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	COHERENTUIPLUGIN_API virtual FReply OnDragDetected( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
#if defined	(COHERENT_UI_UE4_4_5_SUPPORT)
	COHERENTUIPLUGIN_API virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyboardEvent& KeyEvent) override;
	COHERENTUIPLUGIN_API virtual FReply OnKeyUp(const FGeometry& MyGeometry, const FKeyboardEvent& KeyEvent) override;
#else
	COHERENTUIPLUGIN_API virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent) override;
	COHERENTUIPLUGIN_API virtual FReply OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent) override;
#endif
	COHERENTUIPLUGIN_API virtual FReply OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& InCharacterEvent) override;
	COHERENTUIPLUGIN_API virtual bool SupportsKeyboardFocus() const override;

#if defined (COHERENT_UI_UE4_4_5_SUPPORT)
	COHERENTUIPLUGIN_API virtual FReply OnKeyboardFocusReceived(const FGeometry& MyGeometry, const FKeyboardFocusEvent& InKeyboardFocusEvent) override;
	COHERENTUIPLUGIN_API virtual void OnKeyboardFocusLost(const FKeyboardFocusEvent& InKeyboardFocusEvent) override;
	COHERENTUIPLUGIN_API virtual void OnKeyboardFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath) override;
#else
	COHERENTUIPLUGIN_API virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InKeyboardFocusEvent) override;
	COHERENTUIPLUGIN_API virtual void OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath) override;
	COHERENTUIPLUGIN_API virtual void OnFocusLost(const FFocusEvent& InKeyboardFocusEvent) override;
#endif

	COHERENTUIPLUGIN_API void SetRaycastCollisionGroup(ECollisionChannel CollisionChannel);
	COHERENTUIPLUGIN_API void SetRaycastTextureAddressMode(TextureAddress TextureAddressMode);
	COHERENTUIPLUGIN_API void SetRaycastQuality(EInputWidgetRaycastQuality RaycastQuality);
	COHERENTUIPLUGIN_API void SetRaycastUVChannel(uint32 UVChannel);

	DEPRECATED(4.6, "SetForceAcceptInputEvents is deprecated. Use SetAlwaysAcceptMouseInputEvents instead.")
	COHERENTUIPLUGIN_API void SetForceAcceptInputEvents(bool Accept);
	COHERENTUIPLUGIN_API void SetAlwaysAcceptMouseInputEvents(bool bAccept);
	COHERENTUIPLUGIN_API void SetPropagateKeyboardEvents(bool bPropagate);
	COHERENTUIPLUGIN_API void SetLineTraceMode(EInputWidgetLineTraceMode Mode);

	COHERENTUIPLUGIN_API void RefocusView(UCoherentBaseComponent* NewFocusedView);

	FCoherentInputWidgetMouseButtonDown OnCoherentInputWidgetMouseButtonDown;
	FCoherentInputWidgetMouseButtonUp OnCoherentInputWidgetMouseButtonUp;
	FCoherentInputWidgetKeyDown OnCoherentInputWidgetKeyDown;
	FCoherentInputWidgetKeyUp OnCoherentInputWidgetKeyUp;

private:
	Coherent::UI::KeyEventData MakeKeyEvent(const FInputEvent& Event, Coherent::UI::KeyEventData::EventType Type);
	void ProcessKeyEvent(const FInputEvent& Event, Coherent::UI::KeyEventData::EventType Type);

	void GetMouseDataFromFPointerEvent(Coherent::UI::MouseEventData& Data, const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, Coherent::UI::MouseEventData::EventType EventType);

	FReply OnMouseEvent(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, Coherent::UI::MouseEventData::EventType EventType);

	FReply DiscardViewFocus(Coherent::UI::MouseEventData::EventType EventType);

private:
	ECollisionChannel RaycastCollisionChannel;
	TextureAddress RaycastTextureAddressMode;
	EInputWidgetRaycastQuality RaycastQuality;
	uint32 UVChannel;
	bool bHasKeyboardFocus;
	bool bAlwaysAcceptMouseInputEvents;
	bool bPropagateKeyboardEvents;
	TEnumAsByte<EInputWidgetLineTraceMode> LineTraceMode;

	TWeakObjectPtr<AActor> OwnerActor;
	TWeakObjectPtr<UCoherentBaseComponent> FocusedViewComponent;
};