// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLDataHolder.h"
#include "POTLGameInstance.h"
#include "POTLStructure.h"
#include "Components/UEventComponent.h"
#include "Actors/StructureBuilder/AStructureBuilder.h"
#include "Actors/AProFow.h"
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
	StructureBuilder = nullptr;
	BaseRotation = 0;
	BuildBroadcastRootHex = nullptr;
	BuildValid = false;
	BuildMsg = "";

	EventComponent = CreateDefaultSubobject<UEventComponent>(TEXT("EventComponent"));
	if (EventComponent)
	{
		AddOwnedComponent(EventComponent);
	}
}


void APOTLPlayerController::RotateStructure()
{
	//BaseRotation = (BaseRotation + 1) % 6;
	//!! Is a copy of the logic from tick !!//
	if (ActiveToolType == EToolType::PlantStructure)
	{
		if (StructureBuilder)
		{
			StructureBuilder->Rotate(1);
		}
		/*
		if (BuilderStructure && GameInstance)
		{
			BuilderStructure->RemoveStructure();
		}
		if (!CachedHex->AttachedBuilding) {
			BuilderStructure = GameInstance->PlantPlaceholderStructure(CachedHex->HexCubeCoords, BaseRotation, BuildStructureData.Id, nullptr, false);
		}
		*/
	}
}
void APOTLPlayerController::SetToolType(EToolType ToolType)
{
	if (ActiveToolType != ToolType) 
	{
		// Unload previous tool
		if (ActiveToolType == EToolType::PlantStructure)
		{
			if (StructureBuilder)
			{
				StructureBuilder->Hide();
			}

			/*
			if (BuilderStructure)
			{
				BuilderStructure->RemoveStructure();
			}
			for (auto& Hex : ValidStructurePlaceHexes)
			{
				if (Hex)
				{
					Hex->HideDecal();
				}
			}
			ValidStructurePlaceHexes.Empty();
			*/
		}
		if (ActiveToolType == EToolType::Select)
		{
			//DeselectSelectedStructures();
			DeselectAllStructures();
		}

		// Load new tool
		if (ToolType == EToolType::PlantStructure)
		{
			if (StructureBuilder)
			{
				StructureBuilder->Show();
			}

			/*
			if (GameInstance)
			{
				if (BuildStructureData.AttachTo.Num() > 0)
				{
					// Get all structures of AttachTo ids
					TArray<AActor*> AttachToStructures;
					for (auto& StructureId : BuildStructureData.AttachTo)
					{
						FST_Structure* StructureData = GameInstance->GetStructureRowData(StructureId);
						if (StructureData)
						{
							TArray<AActor*> FoundStructures;
							UGameplayStatics::GetAllActorsOfClass(GetWorld(), StructureData->StructureClass, FoundStructures);
							AttachToStructures.Append(FoundStructures);
						}
					}
					for (auto& Actor : AttachToStructures)
					{
						APOTLStructure* Structure = Cast<APOTLStructure>(Actor);
						if (Structure)
						{
							TArray<UHexTile*> AdjacentHexes = UPOTLUtilFunctionLibrary::GetAdjacentHexesToHexes(Structure->OccupiedHexes);
							AdjacentHexes = UPOTLUtilFunctionLibrary::SubtractHexes(AdjacentHexes, Structure->OccupiedHexes);
							for (auto& Hex : AdjacentHexes)
							{
								if (Hex && !Hex->AttachedBuilding) // If no structure
								{
									ValidStructurePlaceHexes.AddUnique(Hex);
								}
							}
						}
					}
					for (auto& Hex : ValidStructurePlaceHexes)
					{
						if (Hex)
						{
							Hex->ShowDecal(EDecalType::ValidBuild);
						}
					}
				}
				else
				{
					// Can be built everywhere
				}
			}
			*/
		}
		else if (ToolType == EToolType::ToggleAllocateHex)
		{

		}

		ActiveToolType = ToolType;
	}
}

void APOTLPlayerController::SelectStructure(APOTLStructure* Structure)
{
	if (Structure)
	{
		DeselectAllStructures();
		Structure->Select();
		/*
		DeselectSelectedStructures();
		Structure->Select();
		SelectedStructures.Add(Structure);
		OnStructureSelected.Broadcast(CachedHex->AttachedBuilding);
		*/
	}
}

void APOTLPlayerController::DeselectSelectedStructures()
{
	for (auto& Structure : SelectedStructures)
	{
		if (Structure)
		{
			Structure->Deselect();
		}
	}
	SelectedStructures.Empty();
}
void APOTLPlayerController::DeselectAllStructures()
{
	for (TActorIterator<APOTLStructure> StructureItr(GetWorld()); StructureItr; ++StructureItr)
	{
		StructureItr->Deselect();
	}
}
void APOTLPlayerController::EditStructure(APOTLStructure* Structure)
{
	if (Structure)
	{
		/*
		ActiveStructure = Structure;
		SetToolType(EToolType::ToggleAllocateHex);
		SelectStructure(Structure);
		*/
	}
}

void APOTLPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	//GameInstance->OnMapReady.AddDynamic();

	StructureBuilder = GetWorld()->SpawnActor<AStructureBuilder>(AStructureBuilder::StaticClass());


	/*
	AProFow* FogOfWar = GetWorld()->SpawnActor<AProFow>(AProFow::StaticClass());
	if (FogOfWar)
	{
		FogOfWar->revealSmoothCircle(FVector2D(700, 300), 800);
	}
	*/

}
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

			//~~ Tool : PlantStructure ~~//
			if (ActiveToolType == EToolType::PlantStructure)
			{
				if (StructureBuilder)
				{
					StructureBuilder->SetRootHex(CachedHex);
				}
				/*
				if (BuilderStructure)
				{
					BuilderStructure->RemoveStructure();
				}
				if (!CachedHex->AttachedBuilding) {
					BuilderStructure = GameInstance->PlantPlaceholderStructure(CachedHex->HexCubeCoords, BaseRotation, BuildStructureData.Id, nullptr, false);
				}
				*/
			}
			//~~ Tool : Select ~~//
			else if (ActiveToolType == EToolType::Select)
			{

			}
		}
	}
}
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

/******************** Input functions *************************/

void APOTLPlayerController::LeftClickPressed()
{
	LeftMouseButtonDown = true;
	if (GameInstance && GameInstance->HexGridReady)
	{
		if (IsValid(CachedHex))
		{
			if (ActiveToolType == EToolType::PlantStructure)
			{
				if (StructureBuilder)
				{
					StructureBuilder->Build();
				}

				/*
				UHexTile* TracedHex = GameInstance->MouseToHex();
				if (TracedHex)
				{	
					if (BuildValid)
					{

					}
					//TODO: Better logic for placement than using AttachTo
					if (BuildStructureData.AttachTo.Num() > 0)
					{
						if (!ValidStructurePlaceHexes.Contains(TracedHex))
						{
							return;
						}
					}
					FST_BuildInfo BuildInfo = FST_BuildInfo();
					BuildInfo.RotationDirection = BaseRotation;
					BuildInfo.StructureInfo = BuildStructureData;

					if (TracedHex->IsBuildable(BuildInfo))
					{

					}

					if (GameInstance->ValidatePlaceStructureOnHex(TracedHex, BuildInfo))
					{
						if (BuilderStructure)
						{
							BuilderStructure->RemoveStructure();
						}
						//~~ Plant structure on the avaiable hex ~~//
						APOTLStructure* Structure = GameInstance->PlantStructure(TracedHex->HexCubeCoords, BaseRotation, BuildStructureData.Id, nullptr, false, false);
					}
				}
				*/
			}
			else if (ActiveToolType == EToolType::Select)
			{
				if (CachedHex->AttachedBuilding)
				{
					//SelectStructure(CachedHex->AttachedBuilding);
					DeselectAllStructures();
					CachedHex->AttachedBuilding->ClickStructure();
				}
				else
				{
					CachedHex->ClickHex();
				}

				const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EPhysicalSurface"), true);
				if (EnumPtr)
				{
					FText SurfaceString = EnumPtr->GetEnumText(CachedHex->HexResourceInfo.SurfaceType);
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, "trace surface type: " + SurfaceString.ToString());
				}
				//OnHexSelected.Broadcast(CachedHex);
				if (CachedHex->HexResourceInfo.SurfaceType == EPhysicalSurface::SurfaceType1) //?? Is this Grass ??//
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, "Grass");
				}
				else if (CachedHex->HexResourceInfo.SurfaceType == EPhysicalSurface::SurfaceType2) //?? Is this Rock ??//
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, "Rock");
				}
				else if (CachedHex->HexResourceInfo.SurfaceType == EPhysicalSurface::SurfaceType2) //?? Is this Water ??//
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
			/*
			else if (ActiveToolType == EToolType::ToggleAllocateHex)
			{
				if (ActiveStructure)
				{
					CachedHex->OnHexToggleAllocate.Broadcast(CachedHex);
				}
			}
			*/
		}
	}
}
void APOTLPlayerController::LeftClickReleased()
{
	LeftMouseButtonDown = false;

	if (EventComponent)
	{
		EventComponent->OnConfirmEvent.Broadcast();
		EventComponent->TriggerConfirmEvent();
	}
}
void APOTLPlayerController::RightClickPressed()
{
	RightMouseButtonDown = true;
	SetToolType(EToolType::Select);

	if (EventComponent)
	{
		EventComponent->OnCancelEvent.Broadcast();
		EventComponent->TriggerCancelEvent();
	}
}
void APOTLPlayerController::RightClickReleased()
{
	RightMouseButtonDown = false;
}

