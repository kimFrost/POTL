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
#pragma once

#include <SlateCore.h>
#include <IToolTip.h>

class SCoherentSubscription : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SCoherentSubscription)
	{}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	static void ShowWidget(bool bIsModal);
	static void ShowErrorDialog(const FString& ErrorDescription);
	static void ShowActivationSuccessfulDialog();

	static FString GetActivationKeyString();
	static FString GetActivationEMailString();

private:
	FReply OnOKButtonClicked();
	FReply OnCancelButtonClicked();
	FReply OnClose();

	void OnActivationKeyTextCommited(const FText& InText, ETextCommit::Type InCommitType);
	void OnActivationEMailTextCommited(const FText& InText, ETextCommit::Type InCommitType);

	FText GetActivationKeyText() const;
	FText GetActivationEMailText() const;

	static bool GetConfigFileValue(const FString& Key, FString& Value);
	static void SetConfigFileValue(const FString& Key, const FString& Value);

	TSharedPtr<class SButton> OKButton;
	TSharedPtr<class SButton> CancelButton;

	mutable FString ActivationKey;
	mutable FString ActivationEMail;
};