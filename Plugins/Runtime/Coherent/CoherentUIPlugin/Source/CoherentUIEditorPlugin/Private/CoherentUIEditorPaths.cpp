/*
This file is part of Coherent UI, modern user interface library for
games.

Copyright (c) 2012-2015 Coherent Labs AD and/or its licensors. All
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
#include "CoherentUIEditorPluginPrivatePCH.h"

#include "CoherentUIEditorPaths.h"
#include "CoherentUISettings.h"

namespace
{
	class FMaxIndexSearchingDirectoryVisitor : public IPlatformFile::FDirectoryVisitor
	{
	public:
		FMaxIndexSearchingDirectoryVisitor(FString InFilename) : MaxIndex(0)
		{
			this->Filename = InFilename;
		}

		virtual bool Visit(const TCHAR* FilenameOrDirectory,
			bool bIsDirectory) override
		{
			if (bIsDirectory)
			{
				return true;
			}

			FString CurrentFile(FilenameOrDirectory);
			if (CurrentFile.Contains(Filename))
			{
				FString IndexAsString = FPaths::GetBaseFilename(CurrentFile)
					.Replace(*Filename, ANSI_TO_TCHAR(""));
				MaxIndex = FCString::Atoi(*IndexAsString);
			}
			return true;
		}

		int32 GetMaxIndex()
		{
			return MaxIndex;
		}
	private:
		FString Filename;
		int32 MaxIndex;
	};
}

void FCoherentUIEditorPaths::Initialize()
{
	PluginResourcesDir = FPaths::Combine(*FPaths::EnginePluginsDir(),
		ANSI_TO_TCHAR("Runtime"),
		ANSI_TO_TCHAR("Coherent"),
		ANSI_TO_TCHAR("CoherentUIPlugin"),
		ANSI_TO_TCHAR("Resources"));

	GameContentDir = FPaths::GameContentDir();

	GameUIResourcesDir = FPaths::Combine(*FPaths::GameContentDir(),
		ANSI_TO_TCHAR("uiresources"));

	GameJSDir = FPaths::Combine(*GameUIResourcesDir, ANSI_TO_TCHAR("javascript"));

	AssetsInstalledDir = FPaths::Combine(*PluginResourcesDir, ANSI_TO_TCHAR("assets"));

	DefaultHTMLPagePath = FPaths::Combine(*PluginResourcesDir,
		ANSI_TO_TCHAR("uiresources"),
		ANSI_TO_TCHAR("default_page_template.html"));

	CoherentJSPath = FPaths::Combine(*PluginResourcesDir,
		ANSI_TO_TCHAR("uiresources"),
		ANSI_TO_TCHAR("javascript"),
		ANSI_TO_TCHAR("coherent.js"));

	HUDHTMLPagePath = FPaths::Combine(*PluginResourcesDir,
		ANSI_TO_TCHAR("uiresources"),
		ANSI_TO_TCHAR("hud.html"));

	DocsURL = FString("http://coherent-labs.com/Documentation/UnrealEngine4/");
	TutorialsURL = FString("http://coherent-labs.com/tutorials");

	CoherentJS = FString("coherent.js");
	DefaultPageName = FString("coherent_view_");
	HUDHTMLPageName = FString("hud.html");

	FMaxIndexSearchingDirectoryVisitor DirectoryVisitor(DefaultPageName);
	FPlatformFileManager::Get().GetPlatformFile()
		.IterateDirectory(*GameUIResourcesDir, DirectoryVisitor);
	PageCounter = DirectoryVisitor.GetMaxIndex() + 1;

	CoherentUIPlane = "CoUIPlane.uasset";
	CoherentUIPlaneReference = "Blueprint'/Game/CoherentUIPlane.CoherentUIPlane'";

	CoherentUIHUDBP = "CoherentUIHUDBP.uasset";
	CoherentUIHUDBPReference = "Blueprint'/Game/CoherentUIHUDBP.CoherentUIHUDBP'";
}

FString FCoherentUIEditorPaths::GetBrowserURL(const FString& Path)
{
	return FString("file://") +
		FPaths::ConvertRelativePathToFull(Path);
}

FString FCoherentUIEditorPaths::GetNewPageName()
{
	return DefaultPageName + FString::FromInt(PageCounter++) +
		FString(".html");
}

FString FCoherentUIEditorPaths::GetDebuggerPath()
{
	return GetDefault<UCoherentUISettings>()->DebuggerPath;
}