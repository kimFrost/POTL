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

#include "ISettingsViewer.h"
#include "ISettingsModule.h"

#include "CoherentUIGameHUD.h"

#include "CoherentUIEditorCommands.h"

FCoherentUIEditorPaths FCoherentUIEditorCommands::UIPaths;

namespace
{
	void EnsureDirectoryExists(const FString& Directory)
	{
		FPlatformFileManager::Get().GetPlatformFile().CreateDirectoryTree(*Directory);
	}

	void EnsurePluginResourcesExist(const FCoherentUIEditorPaths& UIPaths)
	{
		const FString PlanePath = FPaths::Combine(*UIPaths.GameContentDir, *UIPaths.CoherentUIPlane);
		bool bPlaneExists = FPlatformFileManager::Get().GetPlatformFile()
			.FileExists(*PlanePath);
		if (bPlaneExists)
			return;

		FPlatformFileManager::Get().GetPlatformFile()
			.CopyDirectoryTree(*UIPaths.GameContentDir, *UIPaths.AssetsInstalledDir, false);
	}

	// The next 5 functions are copied from GameModeInfoCustomizer. GetCurrentGameModeClass
	// has been slightly altered.

	/** Get the currently set GameMode class */
	const UClass* GetCurrentGameModeClass()
	{
		// Check if there's a selected mode first
		check(GEditor);
		UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
		if (EditorWorld)
		{
			AWorldSettings* WorldSettings = EditorWorld->GetWorldSettings();
			if (WorldSettings->DefaultGameMode)
			{
				const UClass* GameModeClass = WorldSettings->DefaultGameMode->GetAuthoritativeClass();
				if (GameModeClass)
				{
					return GameModeClass;
				}
			}
		}

		// Check global default
		FString ClassName =
			GetMutableDefault<UGameMapsSettings>()->GetGlobalDefaultGameMode();

		if (ClassName.IsEmpty() || ClassName == "None")
		{
			return NULL;
		}

		FString StrippedClassName = ClassName;
		ConstructorHelpers::StripObjectClass(StrippedClassName);

		const UClass* GameModeClass = FindObject<UClass>(ANY_PACKAGE, *StrippedClassName);
		if (!GameModeClass)
		{
			GameModeClass = LoadObject<UClass>(NULL, *ClassName);
		}

		return GameModeClass;
	}

	/** Are we allowed to modify the currently selected GameMode */
	bool AllowModifyGameMode()
	{
		// Only allow editing GameMode BP, not native class!
		const UBlueprintGeneratedClass* GameModeBPClass = Cast<UBlueprintGeneratedClass>(GetCurrentGameModeClass());
		return (GameModeBPClass != NULL);
	}

	/** Get the CDO from the currently set GameMode class */
	AGameMode* GetCurrentGameModeCDO()
	{
		UClass* GameModeClass = const_cast<UClass*>(GetCurrentGameModeClass());
		if (GameModeClass != NULL)
		{
			return GameModeClass->GetDefaultObject<AGameMode>();
		}
		else
		{
			return NULL;
		}
	}

	/** Find the current default class by property name */
	const UClass* GetDefaultClass(FName ClassPropertyName)
	{
		UClass* CurrentDefaultClass = NULL;
		const UClass* GameModeClass = GetCurrentGameModeClass();
		if (GameModeClass != NULL)
		{
			UClassProperty* ClassProp = FindFieldChecked<UClassProperty>(GameModeClass, ClassPropertyName);
			CurrentDefaultClass = (UClass*)ClassProp->GetObjectPropertyValue(ClassProp->ContainerPtrToValuePtr<void>(GetCurrentGameModeCDO()));
		}
		return CurrentDefaultClass;
	}

	/** Set a new default class by property name */
	void SetDefaultClass(const UClass* NewDefaultClass, FName ClassPropertyName)
	{
		const UClass* GameModeClass = GetCurrentGameModeClass();
		if (GameModeClass != NULL && AllowModifyGameMode())
		{
			UClassProperty* ClassProp = FindFieldChecked<UClassProperty>(GameModeClass, ClassPropertyName);
			const UClass** DefaultClassPtr = ClassProp->ContainerPtrToValuePtr<const UClass*>(GetCurrentGameModeCDO());
			*DefaultClassPtr = NewDefaultClass;

			// Indicate that the BP has changed and would need to be saved.
			GameModeClass->MarkPackageDirty();
		}
	}

}

PRAGMA_DISABLE_OPTIMIZATION

void FCoherentUIEditorCommands::RegisterCommands()
{
	UIPaths.Initialize();

#define LOCTEXT_NAMESPACE "MainCoherentUIMenu"
	UI_COMMAND(AddHUD, "Add HUD", "Changes the current HUD class to CoherentUIGameHUD and initialize it. (Works only if the current GameMode is a BP class)", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(AddPlane, "Add Plane", "Adds a 3D plane in front of the camera with a Coherent UI view attached to it.", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(AddPage, "Add HTML Page", "Adds an HTML page to your Content\\uiresources folder.", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(LaunchDebugger, "Launch Debugger", "Launches Coherent UI's Debugger to help you debug your UI. (Works only while the game is running.)", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(LaunchDocs, "Open Documentation", "Opens Coherent UI's documentation.", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(LaunchTutorials, "Open Tutorials", "Opens Coherent UI's tutorials.", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(ShowOptions, "Options", "Open Coherent UI's options", EUserInterfaceActionType::Button, FInputGesture());
#undef LOCTEXT_NAMESPACE
}

bool FCoherentUIEditorCommands::CanExecute::AddHUD()
{
	return AllowModifyGameMode();
}

void FCoherentUIEditorCommands::Execute::AddHUD()
{
	auto GameMode = GetCurrentGameModeClass();
	if (GameMode)
	{
		EnsurePluginResourcesExist(UIPaths);
		const FStringAssetReference HUDReference = UIPaths.CoherentUIHUDBPReference;
		UBlueprint* HUDBlueprint = Cast<UBlueprint>(StaticLoadObject(
		    UBlueprint::StaticClass(), nullptr, *HUDReference.ToString()));
		if (!HUDBlueprint)
		{
			UE_LOG(LogCoherentUIEditor, Warning, TEXT("%s is missing from content directory."), *UIPaths.CoherentUIHUDBP);
			return;
		}
		SetDefaultClass(HUDBlueprint->GeneratedClass, "HUDClass");
		const FString HUDDestinationPath = FPaths::Combine(*UIPaths.GameUIResourcesDir,
			*UIPaths.HUDHTMLPageName);
		bool bHUDExists = FPlatformFileManager::Get().GetPlatformFile()
			.FileExists(*HUDDestinationPath);
		if (!bHUDExists)
		{
			FPlatformFileManager::Get().GetPlatformFile()
				.CopyFile(*HUDDestinationPath, *UIPaths.HUDHTMLPagePath);
		}
		FString PageURL = UIPaths.GetBrowserURL(HUDDestinationPath);
		FPlatformProcess::LaunchURL(*PageURL, nullptr, nullptr);
	}
	else
	{
		UE_LOG(LogCoherentUIEditor, Warning, TEXT("Failed to change HUD Class to CoherentUIGameHUD."));
	}
}

void FCoherentUIEditorCommands::Execute::AddPlane()
{
	EnsurePluginResourcesExist(UIPaths);
	const FStringAssetReference PlaneReference = UIPaths.CoherentUIPlaneReference;
	UBlueprint* PlaneBlueprint = Cast<UBlueprint>(StaticLoadObject(
		UBlueprint::StaticClass(), nullptr, *PlaneReference.ToString()));
	bool bSuccess = false;
	if (PlaneBlueprint && GWorld)
	{
		AActor* PlaneActor = GWorld->SpawnActor<AActor>(PlaneBlueprint->GeneratedClass);
		bSuccess = PlaneActor != nullptr;

		auto ViewportClient = static_cast<FEditorViewportClient*>(
			GEditor->GetActiveViewport()->GetClient());
		FVector CameraPosition = ViewportClient->GetViewLocation();
		FVector CameraDirection = ViewportClient->GetLookAtLocation() - CameraPosition;
		CameraDirection.Normalize();
		GEditor->MoveActorInFrontOfCamera(*PlaneActor, CameraPosition, CameraDirection);

		FVector PlaneToCamera = CameraPosition - PlaneActor->GetActorLocation();
		PlaneToCamera.Z = 0;
		PlaneToCamera.Normalize();
		auto PlaneLookAtRotation = FQuat::FindBetween(PlaneActor->GetActorForwardVector(), PlaneToCamera);
		PlaneLookAtRotation *= FQuat::MakeFromEuler(FVector(0, -90, 0));

		PlaneActor->SetActorRotation(FRotator(PlaneLookAtRotation));
		GEditor->SelectActor(PlaneActor, true, true, true);
		ViewportClient->bNeedsRedraw = true;
	}
	if (!bSuccess)
	{
		UE_LOG(LogCoherentUIEditor, Warning, TEXT("Could not spawn Coherent UI Plane!"));
	}
}

void FCoherentUIEditorCommands::Execute::AddPage()
{
	const FString PageName = UIPaths.GetNewPageName();

	const FString PageDestinationPath = FPaths::Combine(*UIPaths.GameUIResourcesDir,
		*PageName);
	const FString CoherentJSDestinationPath = FPaths::Combine(*UIPaths.GameJSDir,
		*UIPaths.CoherentJS);

	EnsureDirectoryExists(UIPaths.GameUIResourcesDir);
	EnsureDirectoryExists(UIPaths.GameJSDir);

	FPlatformFileManager::Get().GetPlatformFile()
		.CopyFile(*PageDestinationPath, *UIPaths.DefaultHTMLPagePath);

	FPlatformFileManager::Get().GetPlatformFile()
		.CopyFile(*CoherentJSDestinationPath, *UIPaths.CoherentJSPath);

	FString PageURL = UIPaths.GetBrowserURL(PageDestinationPath);
	FPlatformProcess::LaunchURL(*PageURL, nullptr, nullptr);
}

bool FCoherentUIEditorCommands::CanExecute::LaunchDebugger()
{
	return GEditor->PlayWorld != nullptr;
}

void FCoherentUIEditorCommands::Execute::LaunchDebugger()
{
	const FString Path = UIPaths.GetDebuggerPath();
	bool bDebuggerExists = FPlatformFileManager::Get().GetPlatformFile()
		.FileExists(*Path);
	if (!bDebuggerExists)
	{
		UE_LOG(LogCoherentUIEditor, Error, TEXT("Could not start Coherent UI's Debugger - debugger wasn't found at:  %s"), *UIPaths.GetDebuggerPath());
		return;
	}
	const FString WorkingDirectory = FPaths::GetPath(Path);
	FPlatformProcess::CreateProc(*Path, ANSI_TO_TCHAR(""),
		true, false, false, nullptr, 0, *WorkingDirectory, nullptr);
}


void FCoherentUIEditorCommands::Execute::LaunchDocs()
{
	FPlatformProcess::LaunchURL(*UIPaths.DocsURL, nullptr, nullptr);
}

void FCoherentUIEditorCommands::Execute::LaunchTutorials()
{
	FPlatformProcess::LaunchURL(*UIPaths.TutorialsURL, nullptr, nullptr);
}

void FCoherentUIEditorCommands::Execute::ShowOptions()
{
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (SettingsModule)
	{
		SettingsModule->ShowViewer(FName("Project"), FName("Plugins"), FName("Coherent UI"));
	}
}

PRAGMA_ENABLE_OPTIMIZATION
