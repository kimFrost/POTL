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
	APOTLHUD* HUD = Cast<APOTLHUD>(GetHUD());
	if (HUD)
	{
		BuildStructureHexes.Empty();
		HUD->ClearDecals(ConstructDecals);
		TArray<FVector> RotatedCubes = UPOTLUtilFunctionLibrary::RotateCubes(BuildStructureData.CubeSizes, BaseRotation, FVector(0, 0, 0));
		FVector RotatedBroadcastRoot = UPOTLUtilFunctionLibrary::RotateCube(BuildStructureData.BroadcastRoot, BaseRotation, FVector(0, 0, 0));
		for (int32 i = 0; i < RotatedCubes.Num(); i++)
		{
			FVector& Cube = RotatedCubes[i];
			FVector CubeInWorld = Cube + CachedHex.HexCubeCoords;
			FVector2D GlobalAxial = UPOTLUtilFunctionLibrary::ConvertCubeToOffset(CubeInWorld);
			int32 HexIndex = UPOTLUtilFunctionLibrary::GetHexIndex(GlobalAxial, GameInstance->GridXCount);
			if (GameInstance->Hexes.IsValidIndex(HexIndex))
			{
				FST_Hex& Hex = GameInstance->Hexes[HexIndex];
				BuildStructureHexes.Add(Hex);
				EHighlightType Type = EHighlightType::Green;
				if (CubeInWorld == RotatedBroadcastRoot + CachedHex.HexCubeCoords)
				{
					Type = EHighlightType::Blue;
				}
				AHexDecal* Decal = HUD->HighlightHex(Hex, Type);
				ConstructDecals.Add(Decal);
			}
		}
		FVector2D GlobalAxial = UPOTLUtilFunctionLibrary::ConvertCubeToOffset(RotatedBroadcastRoot + CachedHex.HexCubeCoords);
		int32 HexIndex = UPOTLUtilFunctionLibrary::GetHexIndex(GlobalAxial, GameInstance->GridXCount);
		if (GameInstance->Hexes.IsValidIndex(HexIndex))
		{
			BuildBroadcastRootHex = &GameInstance->Hexes[HexIndex];
		}
		if (BuildBroadcastRootHex)
		{
			BuildValid = true;
			if (BuildBroadcastRootHex->AttachedBuilding)
			{
				//~~ Detect if in range of broadcast grid ~~//
				BuildValid = BuildBroadcastRootHex->AttachedBuilding->InRangeOfEmitTo; 
				if (!BuildValid)
				{
					BuildMsg = "Building Root is outside of broadcast grid";
				}
			}
			else
			{
				BuildValid = false;
				BuildMsg = "AttachedBuilding is Nullptr";
			}
			//~~ Validate build spaces are buildable ~~//
			for (int32 i = 0; i < BuildStructureHexes.Num(); i++)
			{
				FST_Hex& Hex = BuildStructureHexes[i];
				bool Buildable = GameInstance->IsHexBuildable(Hex);
				if (!Buildable)
				{
					BuildValid = false;
					BuildMsg = "Building spaces are not buildable";
				}
			}
			//~~ Draw hightlights ~~//
			for (int32 i = 0; i < CityConstructionLocations.Num(); i++)
			{
				FST_Hex& Hex = CityConstructionLocations[i];
				EHighlightType Type = EHighlightType::Green;
				if (Hex.ConstructInfo.Blocked)
				{
					Type = EHighlightType::Red;
				}
				AHexDecal* Decal = HUD->HighlightHex(Hex, Type);
				ConstructDecals.Add(Decal);
			}
			//~~ Display error msg if build is not valid ~~//
			if (!BuildValid)
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, BuildMsg);
			}
		}
		else
		{
			BuildValid = false;
			BuildMsg = "BuildBroadcastRootHex is Nullptr";
		}
	}
}


/******************** RotateStructure *************************/
void APOTLPlayerController::RotateStructure()
{
	BaseRotation = (BaseRotation + 1) % 6;
	//!! Is a copy of the logic from tick !!//
	if (ActiveToolType == EToolType::PlantStructure)
	{
		APOTLStructure* City = GameInstance->GetNearestCity(CachedHex.Location);
		if (City)
		{
			if (!CachedHex.AttachedBuilding) { //!! Might not be right !!//
				if (BuilderStructure)
				{
					//BuilderStructure->Destroy();
					GameInstance->RemoveStructure(BuilderStructure);
				}
				BuilderStructure = GameInstance->PlantPlaceholderStructure(CachedHex.HexCubeCoords, BaseRotation, BuildStructureData.Id, City->TreeId, City, false);
				CityConstructionLocations = GameInstance->GetConstructLocations(City, true);
				ProcessConstructLocations();
			}
			else {
				if (BuilderStructure)
				{
					//BuilderStructure->Destroy();
					GameInstance->RemoveStructure(BuilderStructure);
				}
			}
		}
	}
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
		FST_Hex TracedHex = GameInstance->MouseToHex(); //!! A copy of the hex !!// //?? Make to * ref ??//
		if (TracedHex.HexIndex != CachedHex.HexIndex && TracedHex.HexIndex != -1)
		{
			CachedHex = TracedHex;
			OnHexOver.Broadcast(CachedHex); //~~ Call hex over event dispatcher ~~//
			if (ActiveToolType == EToolType::PlantStructure)
			{
				APOTLStructure* City = GameInstance->GetNearestCity(CachedHex.Location);
				if (City)
				{
					if (BuilderStructure)
					{
						GameInstance->RemoveStructure(BuilderStructure);
					}
					if (!CachedHex.AttachedBuilding) {
						BuilderStructure = GameInstance->PlantPlaceholderStructure(CachedHex.HexCubeCoords, BaseRotation, BuildStructureData.Id, City->TreeId, City, false);
						CityConstructionLocations = GameInstance->GetConstructLocations(City, true);
						ProcessConstructLocations();
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
		if (ActiveToolType == EToolType::PlantStructure)
		{
			if (BuildValid)
			{
				FST_Hex TracedHex = GameInstance->MouseToHex(); //!! A copy of the hex !!//
				//~~ If hex has a placeholder structure on it ~~//
				if (TracedHex.AttachedBuilding && TracedHex.AttachedBuilding->IsPlaceholder) {
					GameInstance->RemoveStructure(TracedHex.AttachedBuilding);
					//~~ Plant structure on the avaiable hex ~~//
					APOTLStructure* City = GameInstance->GetNearestCity(CachedHex.Location);
					if (City)
					{
						GameInstance->PlantStructure(TracedHex.HexCubeCoords, BaseRotation, BuildStructureData.Id, City->TreeId, City, true, false);
						CityConstructionLocations = GameInstance->GetConstructLocations(City, true);
						ProcessConstructLocations();
					}
				}
			}
		}
		else if (ActiveToolType == EToolType::Select)
		{
			OnHexSelected.Broadcast(CachedHex);
			if (CachedHex.Resources.SurfaceType == EPhysicalSurface::SurfaceType1) //?? Is this Grass ??//
			{

			}
			else if (CachedHex.Resources.SurfaceType == EPhysicalSurface::SurfaceType2) //?? Is this Rock ??//
			{

			}
			else if (CachedHex.Resources.SurfaceType == EPhysicalSurface::SurfaceType2) //?? Is this Water ??//
			{

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

