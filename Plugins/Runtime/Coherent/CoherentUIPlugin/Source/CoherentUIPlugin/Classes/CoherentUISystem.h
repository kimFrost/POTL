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
#include "Coherent/Libraries/Logging/Severity.h"
#include "CoherentUISystem.generated.h"

typedef TSharedPtr<CoUISystemWrapper, ESPMode::ThreadSafe> SystemWrapperSharedPtr;

UENUM(BlueprintType)
namespace ELoggingEnum
{
	enum LoggingSeverity
	{
		Trace			UMETA(DisplayName = "Trace"),
		Debug			UMETA(DisplayName = "Debug"),
		Info			UMETA(DisplayName = "Info"),
		Warning			UMETA(DisplayName = "Warning"),
		AssertFailure	UMETA(DisplayName = "AssertFailure"),
		Error			UMETA(DisplayName = "Error")
	};
}

/**
* This Actor encapsulates a Coherent UI System. If you need to modify 
* any of the default values of the System you can put this Actor in the world and it will be
* picked up by all created Views. If no Actor is present, a default one will be created.
*/
UCLASS(MinimalAPI)
class ACoherentUISystem : public AActor
{
	GENERATED_UCLASS_BODY()
public:

	/** Allows setting cookies */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "System Settings")
	bool AllowCookies;
	
	/** The name of the file used for cookies */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "System Settings")
	FString CookiesResource;
	
	/** Enables support for proxy */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "System Settings")
	bool EnableProxy;

	/** Path used for cahced resources */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "System Settings")
	FString CachePath;

	/** Path for HTML5 local storage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "System Settings")
	FString HTML5LocalStoragePath;

	/** Disables fulscreen functionality on all plugins */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "System Settings")
	bool ForceDisablePluginFullscreen;

	/** Disables web security */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "System Settings")
	bool DisableWebSecurity;

	/** Enables support for proprietary codecs. NB: You must provide the proper ffmpeg build yourself. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "System Settings")
	bool SupportProprietaryCodecs;

	/** If true, the system will be updated even when the game is paused */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "System Settings")
	bool UpdateWhenPaused;

	/** Verbosity of the Coherent UI log */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Library log level")
	TEnumAsByte<ELoggingEnum::LoggingSeverity> LogSeverity;

public:
	virtual void TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;

	virtual void PreInitializeComponents() override;

	SystemWrapperSharedPtr GetSystemObject() const;
	
	virtual void BeginDestroy() override;
private:
	SystemWrapperSharedPtr System;
};