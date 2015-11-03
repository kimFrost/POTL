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
#include "CoherentUIEditorCommands.h"
#include "Editor/MainFrame/Public/Interfaces/IMainFrameModule.h"
#include "ISettingsModule.h"
#include "CoherentUISettings.h"
#include "GenericCommands.h"

DEFINE_LOG_CATEGORY(LogCoherentUIEditor);

class FCoherentUIEditorPlugin : public ICoherentUIEditorPlugin
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void MapActions();
	void AddMenuBarExtension(FMenuBarBuilder&);

	TSharedPtr<FUICommandList> PluginCommands;
	TSharedPtr<FExtensibilityManager> ExtensibilityManager;
	TSharedPtr<FExtender> Extender;
	TSharedPtr<const FExtensionBase> Extension;
};

IMPLEMENT_MODULE(FCoherentUIEditorPlugin, CoherentUIEditorPlugin)

namespace
{
#define LOCTEXT_NAMESPACE "MainCoherentUIMenu"
	static void FillCoherentUIMenu(FMenuBuilder& MenuBuilder, const TSharedRef< FExtender > Extender)
	{
		MenuBuilder.BeginSection("CoherentUI_Add", LOCTEXT("CoherentUI_AddHeading", "Add"));
		{
			MenuBuilder.AddMenuEntry(FCoherentUIEditorCommands::Get().AddHUD);
			MenuBuilder.AddMenuEntry(FCoherentUIEditorCommands::Get().AddPlane);
			MenuBuilder.AddMenuEntry(FCoherentUIEditorCommands::Get().AddPage);
		}
		MenuBuilder.EndSection();

		MenuBuilder.BeginSection("CoherentUI_DebugHelp", LOCTEXT("CoherentUI_HelpHeading", "Debug + Help"));
		{
			MenuBuilder.AddMenuEntry(FCoherentUIEditorCommands::Get().LaunchDebugger);
			MenuBuilder.AddMenuEntry(FCoherentUIEditorCommands::Get().LaunchDocs);
			MenuBuilder.AddMenuEntry(FCoherentUIEditorCommands::Get().LaunchTutorials);
		}
		MenuBuilder.EndSection();

		MenuBuilder.BeginSection("CoherentUI_Miscellaneous", LOCTEXT("CoherentUI_MiscellaneousHeading", "Miscellaneous"));
		{
			MenuBuilder.AddMenuEntry(FCoherentUIEditorCommands::Get().ShowOptions);
		}
		MenuBuilder.EndSection();
	}
#undef LOCTEXT_NAMESPACE


	void MapCoherentUIActions(TSharedPtr<FUICommandList> PluginCommands)
	{
		PluginCommands->MapAction(FCoherentUIEditorCommands::Get().AddHUD,
			FExecuteAction::CreateStatic(&FCoherentUIEditorCommands::Execute::AddHUD),
			FCanExecuteAction::CreateStatic(&FCoherentUIEditorCommands::CanExecute::AddHUD));

		PluginCommands->MapAction(FCoherentUIEditorCommands::Get().AddPlane,
			FExecuteAction::CreateStatic(&FCoherentUIEditorCommands::Execute::AddPlane));

		PluginCommands->MapAction(FCoherentUIEditorCommands::Get().AddPage,
			FExecuteAction::CreateStatic(&FCoherentUIEditorCommands::Execute::AddPage));

		PluginCommands->MapAction(FCoherentUIEditorCommands::Get().LaunchDebugger,
			FExecuteAction::CreateStatic(&FCoherentUIEditorCommands::Execute::LaunchDebugger),
			FCanExecuteAction::CreateStatic(&FCoherentUIEditorCommands::CanExecute::LaunchDebugger));

		PluginCommands->MapAction(FCoherentUIEditorCommands::Get().LaunchDocs,
			FExecuteAction::CreateStatic(&FCoherentUIEditorCommands::Execute::LaunchDocs));

		PluginCommands->MapAction(FCoherentUIEditorCommands::Get().LaunchTutorials,
			FExecuteAction::CreateStatic(&FCoherentUIEditorCommands::Execute::LaunchTutorials));

		PluginCommands->MapAction(FCoherentUIEditorCommands::Get().ShowOptions,
			FExecuteAction::CreateStatic(&FCoherentUIEditorCommands::Execute::ShowOptions));
	}

	void RegisterSettings()
	{
#define LOCTEXT_NAMESPACE "CoherentUIEditorSettings"
		if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
		{
			SettingsModule->RegisterSettings("Project", "Plugins", "Coherent UI",
				LOCTEXT("RuntimeSettingsName", "Coherent UI"),
				LOCTEXT("RuntimeSettingsDescription", "Configure the Coherent UI plugin"),
				GetMutableDefault<UCoherentUISettings>()
				);
		}
#undef LOCTEXT_NAMESPACE
	}

	void UnregisterSettings()
	{
		if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
		{
			SettingsModule->UnregisterSettings("Project", "Plugins", "CoherentUI");
		}
	}
}



void FCoherentUIEditorPlugin::StartupModule()
{
	FCoherentUIEditorCommands::Register();
	PluginCommands = MakeShareable(new FUICommandList);
	MapCoherentUIActions(PluginCommands);

	Extender = MakeShareable(new FExtender);
	Extension = Extender->AddMenuBarExtension("Window",
		EExtensionHook::After, PluginCommands, FMenuBarExtensionDelegate::CreateRaw(this, &FCoherentUIEditorPlugin::AddMenuBarExtension));

	RegisterSettings();
	
	FGenericCommands::Register(); // Required for the successful load of FLevelEditorModule
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	ExtensibilityManager = LevelEditorModule.GetMenuExtensibilityManager();
	ExtensibilityManager->AddExtender(Extender);

	UE_LOG(LogCoherentUIEditor, Log, TEXT("Coherent UI Editor plugin initialized!"));
}


void FCoherentUIEditorPlugin::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FCoherentUIEditorCommands::Unregister();
	Extender->RemoveExtension(Extension.ToSharedRef());

	if (ExtensibilityManager.IsValid())
	{
		ExtensibilityManager->RemoveExtender(Extender);
	}
	else
	{
		ExtensibilityManager.Reset();
	}
	UnregisterSettings();

	UE_LOG(LogCoherentUIEditor, Log, TEXT("Coherent UI Editor plugin shut-down!"));
}

void FCoherentUIEditorPlugin::AddMenuBarExtension(FMenuBarBuilder& MenuBuilder)
{
#define LOCTEXT_NAMESPACE "MainMenu"

	UE_LOG(LogCoherentUIEditor, Log, TEXT("Starting Extension logic"));

	MenuBuilder.AddPullDownMenu(
		LOCTEXT("CoherentUIMenu", "Coherent UI"),
		LOCTEXT("CoherentUIMenu_Tooltip", "Open the Coherent UI menu."),
		FNewMenuDelegate::CreateStatic(&FillCoherentUIMenu, Extender.ToSharedRef()),
		"CoherentUIMenu"
		);

#undef LOCTEXT_NAMESPACE

}

