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
	BuildBroadcastRootHex = nullptr;
	BuildValid = false;
	BuildMsg = "";
}



/******************** ProcessConstructLocations *************************/
void APOTLPlayerController::ProcessConstructLocations()
{
	
}


/******************** RotateStructure *************************/
void APOTLPlayerController::RotateStructure()
{
	BaseRotation = (BaseRotation + 1) % 6;
	//!! Is a copy of the logic from tick !!//
	if (ActiveToolType == EToolType::PlantStructure)
	{
		if (BuilderStructure && GameInstance)
		{
			BuilderStructure->RemoveStructure();
		}
		BuilderStructure = GameInstance->PlantPlaceholderStructure(CachedHex->HexCubeCoords, BaseRotation, BuildStructureData.Id, nullptr, false);
		//CityConstructionLocations = GameInstance->GetConstructLocations(City, true);
		ProcessConstructLocations();

		/*
		if (!CachedHex->AttachedBuilding) { //!! Might not be right !!//
			if (BuilderStructure && GameInstance)
			{
				GameInstance->RemoveStructure(BuilderStructure);
			}
			BuilderStructure = GameInstance->PlantPlaceholderStructure(CachedHex->HexCubeCoords, BaseRotation, BuildStructureData.Id, nullptr, false);
			//CityConstructionLocations = GameInstance->GetConstructLocations(City, true);
			ProcessConstructLocations();
		}
		else {
			if (BuilderStructure && GameInstance)
			{
				//BuilderStructure->Destroy();
				GameInstance->RemoveStructure(BuilderStructure);
			}
		}
		*/
	}
}


/******************** SetToolType *************************/
void APOTLPlayerController::SetToolType(EToolType ToolType)
{
	if (ActiveToolType == EToolType::PlantStructure)
	{
		if (BuilderStructure)
		{
			BuilderStructure->RemoveStructure();
		}
	}
	ActiveToolType = ToolType;
}



/******************** BeginPlay *************************/
//~~ Called when the game starts or when spawned ~~//
void APOTLPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	//GameInstance->OnMapReady.AddDynamic();

}


/******************** Tick *************************/
void APOTLPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GameInstance && GameInstance->HexGridReady)
	{
		//GameInstance->MouseToHex
		UHexTile* TracedHex = GameInstance->MouseToHex(); //!! A copy of the hex !!// //?? Make to * ref ??//
		if (IsValid(TracedHex) && TracedHex->HexIndex != -1 && TracedHex != CachedHex)
		{
			CachedHex = TracedHex;
			OnHexOver.Broadcast(CachedHex); //~~ Call hex over event dispatcher ~~//
			if (ActiveToolType == EToolType::PlantStructure)
			{
				if (BuilderStructure)
				{
					BuilderStructure->RemoveStructure();
				}
				if (!CachedHex->AttachedBuilding) {
					BuilderStructure = GameInstance->PlantPlaceholderStructure(CachedHex->HexCubeCoords, BaseRotation, BuildStructureData.Id, nullptr, false);
					//CityConstructionLocations = GameInstance->GetConstructLocations(City, true);
					ProcessConstructLocations();
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

	InputComponent->BindAction("RotateStructure", IE_Pressed, this, &APOTLPlayerController::RotateStructure);

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
	if (GameInstance && GameInstance->HexGridReady)
	{
		if (IsValid(CachedHex))
		{
			if (ActiveToolType == EToolType::PlantStructure)
			{
				//if (BuildValid)
				//{
					UHexTile* TracedHex = GameInstance->MouseToHex();
					if (IsValid(TracedHex))
					{
						//~~ If hex has a placeholder structure on it ~~//
						//if (TracedHex->AttachedBuilding && TracedHex->AttachedBuilding->IsPlaceholder) {
							//GameInstance->RemoveStructure(TracedHex->AttachedBuilding);
							if (BuilderStructure)
							{
								BuilderStructure->RemoveStructure();
							}
							//?? Trace for attach To or make plantstructure handle it??

							//~~ Plant structure on the avaiable hex ~~//
							GameInstance->PlantStructure(TracedHex->HexCubeCoords, BaseRotation, BuildStructureData.Id, nullptr, true, false);
							//CityConstructionLocations = GameInstance->GetConstructLocations(City, true);
							ProcessConstructLocations();
						//}
					}
				//}
			}
			else if (ActiveToolType == EToolType::Select)
			{
				const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EPhysicalSurface"), true);
				if (EnumPtr)
				{
					FText SurfaceString = EnumPtr->GetEnumText(CachedHex->Resources.SurfaceType);
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, "trace surface type: " + SurfaceString.ToString());
				}
				OnHexSelected.Broadcast(CachedHex);
				if (CachedHex->Resources.SurfaceType == EPhysicalSurface::SurfaceType1) //?? Is this Grass ??//
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, "Grass");
				}
				else if (CachedHex->Resources.SurfaceType == EPhysicalSurface::SurfaceType2) //?? Is this Rock ??//
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, "Rock");
				}
				else if (CachedHex->Resources.SurfaceType == EPhysicalSurface::SurfaceType2) //?? Is this Water ??//
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, "Water");
				}

				/*
				FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, PlayerController);
				RV_TraceParams.bTraceComplex = true;
				RV_TraceParams.bTraceAsyncScene = true;
				RV_TraceParams.bReturnPhysicalMaterial = false;

				//Re-initialize hit info
				FHitResult RV_Hit(ForceInit);

				FVector LineTraceFrom = ActorLocation + FVector{ X, Y, 3000 } +FVector{ 1.f, 1.f, 0.f };
				FVector LineTraceTo = ActorLocation + FVector{ X, Y, -3000 } +FVector{ 1.f, 1.f, 0.f };

				PlayerController->GetWorld()->LineTraceSingleByChannel(RV_Hit, LineTraceFrom, LineTraceTo, ChannelLandscape, RV_TraceParams);
				*/
			}	
		}
	}
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
	SetToolType(EToolType::Select);
}

/******************** LeftClickReleased *************************/
void APOTLPlayerController::RightClickReleased()
{
	RightMouseButtonDown = false;
}

