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

#include "CoherentBaseComponent.h"
#include "CoherentUIOptionsMenuControl.h"

#include "CoherentUIOptionsMenu.generated.h"


USTRUCT(Blueprintable)
struct FCoherentUISavedOptions
{
	GENERATED_USTRUCT_BODY()
public:
	/** Name of the saved control */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Options")
	FString Name;

	/** Type of the saved control */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Options")
	FString Type;

	/** Current value of the saved control */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Options")
	FString CurrentValue;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCoherentOptionsSaved, const TArray<FCoherentUISavedOptions>&, SavedOptions);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCoherentOptionSaved, TArray<int32>, SavedOption);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCoherentOptionsDiscarded);

UCLASS(MinimalAPI, BlueprintType, Blueprintable)
class UCoherentUIOptionsMenu : public UObject
{
	GENERATED_UCLASS_BODY()
public:

	UFUNCTION()
	void Initialize(UCoherentBaseComponent* ViewComponent, TArray<FCoherentUIOptionsMenuControl> controls);

	UFUNCTION()
	void OnReadyForScripting(int32 FrameId, const FString& Path, bool IsMainFrame);

  // Note: If you get "LogCompile:Error: Remapping old metadata key"
  // it is safe to ignore in 4.8.0. It is caused by deprecation of the
  // "FriendlyName" metadata key. In order to archieve backwards compatability
  // for assets created with a UE4 version prior to 4.8 the deprecated
  // "Friendly Name" will stay until it creates a compiler error.

	UPROPERTY(BlueprintAssignable, meta = (HidePin = "WorldContextObject",
		DefaultToSelf = "WorldContextObject",
		FriendlyName = "On Options Save"),
		Category = "Menu UI")
		FCoherentOptionsSaved OnMenuOptionsSaved;

  // Note: If you get "LogCompile:Error: Remapping old metadata key"
  // it is safe to ignore in 4.8.0. It is caused by deprecation of the
  // "FriendlyName" metadata key. In order to archieve backwards compatability
  // for assets created with a UE4 version prior to 4.8 the deprecated
  // "Friendly Name" will stay until it creates a compiler error.

  UPROPERTY(BlueprintAssignable, meta = (HidePin = "WorldContextObject",
		DefaultToSelf = "WorldContextObject",
		FriendlyName = "On Options Save"),
		Category = "Menu UI")
		FCoherentOptionSaved OnTestMenuOptionSaved;

  // Note: If you get "LogCompile:Error: Remapping old metadata key"
  // it is safe to ignore in 4.8.0. It is caused by deprecation of the
  // "FriendlyName" metadata key. In order to archieve backwards compatability
  // for assets created with a UE4 version prior to 4.8 the deprecated
  // "Friendly Name" will stay until it creates a compiler error.

	UPROPERTY(BlueprintAssignable, meta = (HidePin = "WorldContextObject",
		DefaultToSelf = "WorldContextObject",
		FriendlyName = "On Options Discarded"),
		Category = "Menu UI")
		FCoherentOptionsDiscarded OnMenuOptionsDiscarded;

private:
	void OnOptionsSaved(TArray<FCoherentUISavedOptions> savedOptions);
	void OnOptionsDiscarded();

	void SetupScripting(Coherent::UI::View* View);
private:
	TArray<FCoherentUIOptionsMenuControl> Controls;

	TWeakObjectPtr<UCoherentBaseComponent> OptionsMenuViewComponent;
};