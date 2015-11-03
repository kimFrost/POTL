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
#include "CoherentUIPluginPrivatePCH.h"
#include "SCoherentSubscription.h"
#include "SlateDelegates.h"
#include "IWidgetReflector.h"
#include "IPlatformTextField.h"
#include "SlateScrollHelper.h"
#include "MenuStack.h"
#include "SVirtualKeyboardEntry.h"
#include "SlateApplication.h"

#if defined (COHERENT_UI_UE4_4_5_SUPPORT)
#include "Slate.h"
#else
#include "SlateCore.h"
#include "SBorder.h"
#include "UIAction.h"
#include "UICommandInfo.h"
#include "UICommandList.h"
#include "MultiBoxExtender.h"

#include "SEditableText.h"
#include "SEditableTextBox.h"

#include "TextRange.h"
#include "IRunRenderer.h"
#include "TextHitPoint.h"
#include "IRun.h"
#include "TextRunRenderer.h"
#include "TextLineHighlight.h"
#include "TextLayout.h"
#include "STextBlock.h"

#include "SButton.h"

#endif

#define LOCTEXT_NAMESPACE "CoherentSubscription"

static const FString GBaseIniName("CoherentUI");
static const FString GSectionName("SubscriptionInfo");
static const FString GActivationEMail("ActivationEMail");
static const FString GActivationKey("ActivationKey");

void SCoherentSubscription::ShowWidget(bool bIsModal)
{
	const FText SubscriptionWindowTitle = LOCTEXT("Coherent UI Subscription", "Coherent UI Subscription Info");

	TSharedPtr<SWindow> SubscriptionWindow =
		SNew(SWindow)
		.Title(SubscriptionWindowTitle)
		.ClientSize(FVector2D(600.f, 200.f))
		.SupportsMaximize(false).SupportsMinimize(false)
		.SizingRule(ESizingRule::FixedSize)
		[
			SNew(SCoherentSubscription)
		];

	if (bIsModal)
	{
		FSlateApplication::Get().AddModalWindow(SubscriptionWindow.ToSharedRef(), FSlateApplication::Get().GetActiveTopLevelWindow());
	}
	else
	{
		FSlateApplication::Get().AddWindow(SubscriptionWindow.ToSharedRef());
	}
}

void SCoherentSubscription::ShowErrorDialog(const FString& ErrorDescription)
{
	FText WarningMessage = FText::Format(LOCTEXT("InvalidLicense", "Coherent UI Subscription license invalid. Error info: \"{0}\".\n\nWould you like to update your credentials (You'll need to restart the game for the changes to take effect)?"), FText::FromString(ErrorDescription));
	if (EAppReturnType::Yes == FMessageDialog::Open(EAppMsgType::YesNo, WarningMessage))
	{
		ShowWidget(true);
	}
}

void SCoherentSubscription::ShowActivationSuccessfulDialog()
{
	FText SuccessMessage(LOCTEXT("SuccessfulActivation", "Coherent UI Subscription version was activated successfully!"));
	FMessageDialog::Open(EAppMsgType::Ok, SuccessMessage);
}

void SCoherentSubscription::Construct(const FArguments& InArgs)
{
	FSlateFontInfo Font = FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 12);

	ChildSlot
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.FillHeight(2.0f)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.FillHeight(1.0f)
				.Padding(2.0f)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("EMailLabel", "E-Mail"))
					.ToolTipText(LOCTEXT("EMailLabel_Tooltip", "The e-mail that identifies your Coherent UI account."))
					.Font(Font)
				]
				+SVerticalBox::Slot()
				.FillHeight(1.0f)
				.Padding(2.0f)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("ActivationKeyLabel", "Activation Key"))
					.ToolTipText(LOCTEXT("ActivationKeyLabel_Tooltip", "The activation key for your Coherent UI subscription."))
					.Font(Font)
				]
			]
			+SHorizontalBox::Slot()
			.FillWidth(2.0f)
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.FillHeight(1.0f)
				.Padding(2.0f)
				[
					SNew(SEditableTextBox)
					.Text(this, &SCoherentSubscription::GetActivationEMailText)
					.ToolTipText(LOCTEXT("EMailLabel_Tooltip", "The e-mail that identifies your Coherent UI account."))
					.OnTextCommitted(this, &SCoherentSubscription::OnActivationEMailTextCommited)
					.OnTextChanged(this, &SCoherentSubscription::OnActivationEMailTextCommited, ETextCommit::Default)
					.Font(Font)
				]
				+SVerticalBox::Slot()
				.FillHeight(1.0f)
				.Padding(2.0f)
				[
					SNew(SEditableTextBox)
					.Text(this, &SCoherentSubscription::GetActivationKeyText)
					.OnTextCommitted(this, &SCoherentSubscription::OnActivationKeyTextCommited)
					.OnTextChanged(this, &SCoherentSubscription::OnActivationKeyTextCommited, ETextCommit::Default)
					.ToolTipText(LOCTEXT("ActivationKeyLabel_Tooltip", "The activation key for your Coherent UI subscription."))
					.Font(Font)
				]
			]
		]
		+SVerticalBox::Slot()
		.FillHeight(2.0f)
		+SVerticalBox::Slot()
		.FillHeight(1.0f)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Bottom)
				[
					SAssignNew(OKButton, SButton)
					.Text(LOCTEXT("OK", "OK"))
					.OnClicked(this, &SCoherentSubscription::OnOKButtonClicked)
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Bottom)
				[
					SAssignNew(CancelButton, SButton)
					.Text(LOCTEXT("Cancel", "Cancel"))
					.OnClicked(this, &SCoherentSubscription::OnCancelButtonClicked)
				]
			]
		]
	];
}

bool SCoherentSubscription::GetConfigFileValue(const FString& Key, FString& Value)
{
	FString DefaultIniName = FString::Printf(TEXT("%s/%s.ini"),
		*FPaths::Combine(*FPaths::GeneratedConfigDir(), ANSI_TO_TCHAR(FPlatformProperties::PlatformName())),
		*GBaseIniName);

	#if defined (COHERENT_UI_UE4_4_5_SUPPORT) || defined (COHERENT_UI_UE4_4_6_SUPPORT)
		FConfigCacheIni Config;
	#else
		FConfigCacheIni Config(EConfigCacheType::DiskBacked);
	#endif

	// Make sure the config file is loaded
	FString FinalIniFileName;
	GConfig->LoadGlobalIniFile(FinalIniFileName, *GBaseIniName);

	bool Result = Config.GetString(*GSectionName, *Key, Value, DefaultIniName);
	return Result;
}

void SCoherentSubscription::SetConfigFileValue(const FString& Key, const FString& Value)
{
	FString DefaultIniName = FString::Printf(TEXT("%s/%s.ini"),
		*FPaths::Combine(*FPaths::GeneratedConfigDir(), ANSI_TO_TCHAR(FPlatformProperties::PlatformName())),
		*GBaseIniName);

	#if defined (COHERENT_UI_UE4_4_5_SUPPORT) || defined (COHERENT_UI_UE4_4_6_SUPPORT)
		FConfigCacheIni Config;
	#else
		FConfigCacheIni Config(EConfigCacheType::DiskBacked);
	#endif

	// Property write
	Config.SetString(*GSectionName, *Key, *Value, DefaultIniName);
	Config.Flush(false);

	// Reload the config file to update the internal structures
	FString FinalIniFileName;
	GConfig->LoadGlobalIniFile(FinalIniFileName, *GBaseIniName);
}

FString SCoherentSubscription::GetActivationKeyString()
{
	FString Result;
	GetConfigFileValue(GActivationKey, Result);
	return Result;
}

FString SCoherentSubscription::GetActivationEMailString()
{
	FString Result;
	GetConfigFileValue(GActivationEMail, Result);
	return Result;
}

//----------------------------------------------------------------------------

FText SCoherentSubscription::GetActivationKeyText() const
{
	if (ActivationKey.IsEmpty())
	{
		GetConfigFileValue(GActivationKey, ActivationKey);
	}

	return FText::FromString(ActivationKey);
}

FText SCoherentSubscription::GetActivationEMailText() const
{
	if (ActivationEMail.IsEmpty())
	{
		GetConfigFileValue(GActivationEMail, ActivationEMail);
	}

	return FText::FromString(ActivationEMail);
}

void SCoherentSubscription::OnActivationKeyTextCommited(const FText& InText, ETextCommit::Type InCommitType)
{
	ActivationKey = InText.ToString();
}

void SCoherentSubscription::OnActivationEMailTextCommited(const FText& InText, ETextCommit::Type InCommitType)
{
	ActivationEMail = InText.ToString();
}

FReply SCoherentSubscription::OnOKButtonClicked()
{
	SetConfigFileValue(GActivationEMail, ActivationEMail);
	SetConfigFileValue(GActivationKey, ActivationKey);
	return OnClose();
}

FReply SCoherentSubscription::OnCancelButtonClicked()
{
	return OnClose();
}

FReply SCoherentSubscription::OnClose()
{
	TSharedRef<SWindow> ParentWindow = FSlateApplication::Get().FindWidgetWindow(AsShared()).ToSharedRef();
	FSlateApplication::Get().RequestDestroyWindow(ParentWindow);

	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE