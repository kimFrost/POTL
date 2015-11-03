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

#include "CoherentUISystem.h"
#include "SCoherentSubscription.h"
#include "Coherent/UI/Declarations.h"
#include "Coherent/UI/ViewContext.h"
#include "CoherentUISettings.h"

ACoherentUISystem::ACoherentUISystem(
#if defined (COHERENT_UI_UE4_4_5_SUPPORT)
	const FPostConstructInitializeProperties& PCIP)
#else
	const FObjectInitializer& PCIP)
#endif
	: Super(PCIP)
{
	AllowCookies = true;
	CookiesResource = TEXT("coui://cookies.dat");
	EnableProxy = false;
	CachePath = TEXT("cui_cache");
	HTML5LocalStoragePath = TEXT("cui_app_cache");
	ForceDisablePluginFullscreen = true;
	DisableWebSecurity = true;
	SupportProprietaryCodecs = false;

	UpdateWhenPaused = true;
	PrimaryActorTick.bTickEvenWhenPaused = UpdateWhenPaused;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ACoherentUISystem::TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);
	if (System.IsValid())
	{
		System->Update();
		ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
			FetchCoherentUISurfaces,
			SystemWrapperSharedPtr, system, System,
			{
				system->FetchSurfaces();
			}
		);
	}
}

void ACoherentUISystem::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	if (GetWorld()->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	Coherent::UI::ContextSettings settings;
	settings.AllowCookies = AllowCookies;
	settings.CookiesResource = *CookiesResource;
	settings.EnableProxy = EnableProxy;
	settings.CachePath = *CachePath;
	settings.HTML5LocalStoragePath = *HTML5LocalStoragePath;
	settings.ForceDisablePluginFullscreen = ForceDisablePluginFullscreen;
	settings.DisableWebSecurity = DisableWebSecurity;
	settings.DebuggerPort = GetDefault<UCoherentUISettings>()->DebuggerPort;

#if defined(COHERENT_UI_SUBSCRIPTION) && defined(UE_EDITOR) && (UE_EDITOR == 1)
	const unsigned EMAIL_LENGTH_LIMIT = 254;
	char email[EMAIL_LENGTH_LIMIT] = { 0 };
	char key[EMAIL_LENGTH_LIMIT] = { 0 };
	{
		FString SubscriptionEMail = SCoherentSubscription::GetActivationEMailString();
		FString SubscriptionKey = SCoherentSubscription::GetActivationKeyString();
		if (SubscriptionEMail.IsEmpty() || SubscriptionKey.IsEmpty())
		{
			SCoherentSubscription::ShowWidget(true);
			SubscriptionEMail = SCoherentSubscription::GetActivationEMailString();
			SubscriptionKey = SCoherentSubscription::GetActivationKeyString();
		}
		FTCHARToUTF8 utf8Email(*SubscriptionEMail);
		FTCHARToUTF8 utf8Key(*SubscriptionKey);

		FMemory::Memcpy(email, (ANSICHAR*)utf8Email.Get(), FMath::Min<unsigned>(utf8Email.Length(), sizeof(email) - 1));
		FMemory::Memcpy(key, (ANSICHAR*)utf8Key.Get(), FMath::Min<unsigned>(utf8Key.Length(), sizeof(key) - 1));
		settings.SubscriptionEmail = email;
		settings.SubscriptionKey = key;
	}
#endif

	int SeverityLevel = static_cast<int>(LogSeverity);
	Coherent::Logging::Severity Severity = static_cast<Coherent::Logging::Severity>(SeverityLevel);

	Coherent::UI::FactorySettings factorySettings;
	factorySettings.EnableSupportForProprietaryCodecs = SupportProprietaryCodecs;


	System = CoUISystemWrapper::CreateContext(factorySettings, settings, Severity);

	PrimaryActorTick.bTickEvenWhenPaused = UpdateWhenPaused;
}

SystemWrapperSharedPtr ACoherentUISystem::GetSystemObject() const
{
	return System;
}

void ACoherentUISystem::BeginDestroy()
{
	Super::BeginDestroy();
	System.Reset();
}
