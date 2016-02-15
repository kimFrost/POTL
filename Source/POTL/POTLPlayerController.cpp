// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLDataHolder.h"
#include "POTLGameInstance.h"
#include "POTLStructure.h"
#include "Kismet/GameplayStatics.h"
#include "POTLHUD.h"
#include "POTLUtilFunctionLibrary.h"
#include "POTLPlayerController.h"





// Sets default values
APOTLPlayerController::APOTLPlayerController(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
	GameInstance = nullptr;
	ActiveToolType = EToolType::Select;
	BuildingAllowed = false;
	BuilderStructure = nullptr;
	BaseRotation = 0;
}





/******************** BeginPlay *************************/
//~~ Called when the game starts or when spawned ~~//
void APOTLPlayerController::BeginPlay()
{
	Super::BeginPlay();
	

}


/******************** Tick *************************/
void APOTLPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GameInstance && GameInstance->HexGridReady)
	{
		//GameInstance->MouseToHex
		FST_Hex TracedHex = GameInstance->MouseToHex(); //!! A copy of the hex !!//
		if (TracedHex.HexIndex != CachedHex.HexIndex)
		{
			CachedHex = TracedHex;
			OnHexOver.Broadcast(CachedHex); //~~ Call hex over event dispatcher ~~//
			//OnHexSelected.Broadcast(CachedHex);
			if (ActiveToolType == EToolType::PlantStructure)
			{
				APOTLStructure* City = GameInstance->GetNearestCity(CachedHex.Location);
				if (City)
				{
					if (!CachedHex.AttachedBuilding) {
						if (BuilderStructure)
						{
							BuilderStructure->Destroy();
						}
						BuilderStructure = GameInstance->PlantPlaceholderStructure(CachedHex.HexCubeCoords, BaseRotation, BuildStructureData.Id, City->TreeId, City, false);
						CityConstructionLocations = GameInstance->GetConstructLocations(City, true);
					}
					else {
						if (BuilderStructure)
						{
							BuilderStructure->Destroy();
						}
					}
				}
			}
			else if (ActiveToolType == EToolType::Select)
			{

			}
		}
	}
}


/******************** SetupPlayerInputComponent *************************/
void APOTLPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	// Set up gameplay key bindings
	check(InputComponent);

	//~~ Mouse ~~//
	InputComponent->BindAction("LeftClick", IE_Pressed, this, &APOTLPlayerController::LeftClickPressed);
	InputComponent->BindAction("LeftClick", IE_Released, this, &APOTLPlayerController::LeftClickReleased);
	InputComponent->BindAction("RightClick", IE_Pressed, this, &APOTLPlayerController::RightClickPressed);
	InputComponent->BindAction("RightClick", IE_Released, this, &APOTLPlayerController::RightClickReleased);

	//InputComponent->BindAction("Rotate", IE_Released, this, &APOTLPlayerController::Rotate);

	//InputComponent->BindAction("LeftClick", IE_Pressed, this, &ACharacter::Jump);
	//InputComponent->BindAxis("MoveForward", this, &ATutorialCodeCharacter::MoveForward);
	//InputComponent->BindTouch(IE_Pressed, this, &ATutorialCodeCharacter::TouchStarted);
}

//~~ Input functions ~~//

/******************** LeftClickPressed *************************/
void APOTLPlayerController::LeftClickPressed()
{
	LeftMouseButtonDown = true;
}

/******************** LeftClickReleased *************************/
void APOTLPlayerController::LeftClickReleased()
{
	LeftMouseButtonDown = false;
}

/******************** LeftClickPressed *************************/
void APOTLPlayerController::RightClickPressed()
{
	RightMouseButtonDown = true;
}

/******************** LeftClickReleased *************************/
void APOTLPlayerController::RightClickReleased()
{
	RightMouseButtonDown = false;
}