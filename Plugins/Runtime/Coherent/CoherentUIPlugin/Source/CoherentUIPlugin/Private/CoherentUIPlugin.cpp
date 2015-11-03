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

DEFINE_LOG_CATEGORY(LogCoherentUI);

class FCoherentUIPlugin : public ICoherentUIPlugin
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

#if PLATFORM_WINDOWS
	HMODULE CoherentUIModule;
#endif
};

IMPLEMENT_MODULE( FCoherentUIPlugin, CoherentUIPlugin )

FString g_CoUIPath;

void FCoherentUIPlugin::StartupModule()
{
	bool result = true;
	g_CoUIPath = FPaths::EngineDir() / TEXT("Binaries/ThirdParty/CoherentUI/");
#if PLATFORM_WINDOWS
	#if PLATFORM_64BITS
	g_CoUIPath /= TEXT("Win64/");
	CoherentUIModule = ::LoadLibraryW(*(g_CoUIPath + TEXT("CoherentUI64.dll")));
	#else	//32-bit platform
	g_CoUIPath /= TEXT("Win32/");
	CoherentUIModule = ::LoadLibraryW(*(g_CoUIPath + TEXT("CoherentUI.dll")));
	#endif

	if (!CoherentUIModule)
	{
		result = false;
		UE_LOG(LogCoherentUI, Error, TEXT("Unable to load Coherent UI library, search path: %s"), *g_CoUIPath);
	}
#elif PLATFORM_MAC
	g_CoUIPath /= TEXT("MacOSX/");
#endif	//PLATFORM_WINDOWS

	if (result)
	{
		UE_LOG(LogCoherentUI, Log, TEXT("CoherentUI plugin initialized!"));
	}
	else
	{
		UE_LOG(LogCoherentUI, Warning, TEXT("CoherentUI plugin initialization failed!"));
	}
}


void FCoherentUIPlugin::ShutdownModule()
{
#if PLATFORM_WINDOWS
	if (CoherentUIModule)
	{
		::FreeLibrary(CoherentUIModule);
		CoherentUIModule = nullptr;
	}
#endif
	UE_LOG(LogCoherentUI, Log, TEXT("CoherentUI plugin shut-down!"));
}
