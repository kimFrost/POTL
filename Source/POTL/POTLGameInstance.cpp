// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLUtilFunctionLibrary.h"
#include "POTLStructure.h"
#include "POTLGameInstance.h"



// Sets default values
UPOTLGameInstance::UPOTLGameInstance(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
	HexWidth = 255.0f;
	HexHeight = HexWidth / FMath::Sqrt(3) * 2.f;
	Landscape = nullptr;
	GridXCount = 200; // Temp. Needs to be calc in point creation.
	GridYCount = 200; // Temp. Needs to be calc in point creation.

	//FindmeString = "set in constructor";
	
	//DataTable'/Game/Resources/TestData.TestData'
	//DataTable'/Game/Resources/ResourceConversion.ResourceConversion'

	//~~ Table data ~~//
	RecipeTable = nullptr;
	StructureTable = nullptr;

	//~~ Recipes ~~//
	static ConstructorHelpers::FObjectFinder<UDataTable>RecipeTable_BP(TEXT("DataTable'/Game/Resources/ResourceRecipies.ResourceRecipies'")); //~~ Get the Uassets file reference ~~//
	if (RecipeTable_BP.Succeeded())
	{
		RecipeTable = RecipeTable_BP.Object;
		FindmeString = "RecipeTable Succeeded : " + FString::FromInt(RecipeTable->GetRowNames().Num());
	}
	
	//~~ Structures ~~//
	//ConstructorHelpersInternal::FindOrLoadObject();
	//template<typename T>
	//T ConstructorHelpersInternal::FindOrLoadObject<UDataTable>(TEXT("DataTable'/Game/Resources/Structures.Structures'"));



	static ConstructorHelpers::FObjectFinder<UDataTable>StructureTable_BP(TEXT("DataTable'/Game/Resources/Structures.Structures'")); //~~ Get the Uassets file reference ~~//
	if (StructureTable_BP.Succeeded())
	{ 
		StructureTable = StructureTable_BP.Object;
		int TempLength = StructureTable->GetRowNames().Num();
		FindmeString = "StructureTable Succeeded : " + FString::FromInt(TempLength);
	}
	else {
		FindmeString = "Failed";
	}


	



	/* 
	if (RecipeTable)
	{
		TArray<FName> RowNames = RecipeTable->GetRowNames();
		static const FString ContextString(TEXT("RowName")); // Key value for each column of values
		FST_ResourceRecipe* LookUpRow = RecipeTable->FindRow<FST_ResourceRecipe>(FName(TEXT("0")), ContextString); // o-- Search using FindRow. It returns a handle to the row.
		// Access the variables like LookUpRow->Blueprint_Class, GOLookupRow->Usecode
		//uint8 SkillsCount = InformationTable->GetTableData().Num(); 
		if (LookUpRow)
		{

		}
	}
	*/
	
}

/******************** GetConstructLocations *************************/
TArray<FST_Hex> UPOTLGameInstance::GetConstructLocations(APOTLStructure* Structure, bool IncludeChildren)
{
	//Log("GetConstructLocations", 15.0f, FColor::Yellow, 2);
	TArray<FST_Hex> ConstructHexes;
	TArray<int32> ConstructHexIndexes;
	ConstructHexIndexes = GetConstructLocationIndexes(Structure, IncludeChildren); //~~ Get self hex indexes ~~//
	//~~ Get children hex indexes ~~//

	/*
	if (IncludeChildren) {
		//Log("Structure->BroadcastTo.Num(): " + FString::FromInt(Structure->BroadcastTo.Num()), 15.0f, FColor::Yellow, 5);
		for (int32 i = 0; i < Structure->BroadcastTo.Num(); i++)
		{
			TArray<int32> ChildrenConstructLocationIndexes = GetConstructLocationIndexes(Structure->BroadcastTo[i], IncludeChildren);
			for (int32 ii = 0; ii < ChildrenConstructLocationIndexes.Num(); ii++)
			{
				ConstructHexIndexes.AddUnique(ChildrenConstructLocationIndexes[ii]);
			}
		}
	}
	*/

	//~~ Convert indexes into real hexes and send a array of copies of them ~~//
	for (int32 h = 0; h < ConstructHexIndexes.Num(); h++)
	{
		ConstructHexes.Add(Hexes[ConstructHexIndexes[h]]);
	}
	Log("ConstructHexes.Num(): " + FString::FromInt(ConstructHexes.Num()), 15.0f, FColor::Yellow, 4);
	return ConstructHexes;
}


/******************** GetConstructLocationIndexes *************************/
TArray<int32> UPOTLGameInstance::GetConstructLocationIndexes(APOTLStructure* Structure, bool IncludeChildren)
{
	TArray<int32> ConstructHexIndexes;
	TArray<int32> VisitedHexIndexes;
	
	struct Frontier
	{
		TArray<int32> HexIndexes;
	};
	TArray<Frontier> Frontiers;

	Log("Structure->HexIndex: " + FString::FromInt(Structure->HexIndex), 15.0f, FColor::Yellow, 4);

	//VisitedHexIndexes.Add(Structure->HexIndex); //~~ Add Start Hex to VisitedHexes ~~//

	/*
	Frontier frontier;
	frontier.HexIndexes.Add(Structure->HexIndex);
	Frontiers.Add(frontier);

	FString TreeId = Structure->TreeId;

	if (IncludeChildren) {
		for (int32 i = 0; i < Structure->BroadcastTo.Num(); i++)
		{
			TArray<int32> ChildrenConstructLocationIndexes = GetConstructLocationIndexes(Structure->BroadcastTo[i], IncludeChildren);
			for (int32 ii = 0; ii < ChildrenConstructLocationIndexes.Num(); ii++)
			{
				ConstructHexIndexes.AddUnique(ChildrenConstructLocationIndexes[ii]);
			}
		}
	}

	for (int32 k = 1; k <= Structure->BroadcastRange + 1; k++)
	{
		Frontier frontier;
		Frontiers.Add(frontier);
		frontier = Frontiers[k - 1];
		for (int32 m = 0; m < frontier.HexIndexes.Num(); m++)
		{
			FST_Hex& Hex = Hexes[frontier.HexIndexes[m]];
			//~~ Reset construct info if the storred tre id ain't the same. ~~//
			if (Hex.ConstructInfo.TreeId != TreeId)
			{
				Hex.ConstructInfo = FST_ConstructLocation{};
			}
			//~~ Make Construct Location ~~//
			Hex.ConstructInfo.Cube = Hex.HexCubeCoords;
			Hex.ConstructInfo.EmitTo.Add(Structure);  // Don't know if it should be a hex or structure reference to, for it to be the best solution.

			//~~ Store broadcasted resources? NO. Will result in a lot of hex updating. Could then result in bugs with imcomplete data. ~~//

			int32 ValidNeighborCount = 0;
			//~~ Add neighbors to the new frontier/next step. Only if they haven't been visited yet. ~~//
			for (int32 i = 0; i < Hex.HexNeighborIndexes.Num(); i++)
			{
				int32 Index = Hex.HexNeighborIndexes[i];
				if (Index != -1 && Hexes.IsValidIndex(Index))
				{
					FST_Hex& NeighborHex = Hexes[Index];
					ValidNeighborCount++;

					if (NeighborHex.AttachedBuilding != nullptr //~~ Only if pointer to structure isn't null ~~//
					&& !Hex.ConstructInfo.AttachTo.Contains(NeighborHex.AttachedBuilding) //~~ Only if structure isn't already stored in attachments. Will cause structure to block for itself ~~//
					&& NeighborHex.AttachedBuilding->TreeId == TreeId) //~~  Only structures in same Tree ~~//
					{
						Hex.ConstructInfo.AttachTo.Add(NeighborHex.AttachedBuilding);
					}
					if (!VisitedHexIndexes.Contains(Index))
					{
						//~~ Search for attachments/adjacent buildings and store them in hexes ~~//
						if (IsHexBuildable(NeighborHex))
						{
							Frontiers[k].HexIndexes.Add(Index); //~~ Add Neighbor Hex to the next frontier ~~//
							VisitedHexIndexes.Add(Index); //~~ Add index to visited indexes, so that neighbors don't overlap each other. ~~//
						}
					}
				}
			}
			//Hex.DebugMe = true;
			if (ValidNeighborCount < 6) //~~ If ValidNeighborCount is less than 6 the hex is on the ridge of the city limit ~~//
			{
				Hex.ConstructInfo.OnRidge = true;
			}
			ConstructHexIndexes.AddUnique(Hex.HexIndex);
		}
	}
	Structure->BroadcastGridHexIndexes = ConstructHexIndexes; //~~ Store indexes in structure. It cludes all hex broadcast grid index, including childrens hex indexes ~~//
	*/
	return ConstructHexIndexes; //~~ Return ~~//
}


/******************** IsHexBuildable *************************/
bool UPOTLGameInstance::IsHexBuildable(FST_Hex& Hex)
{
	//FRotator HexRotation = Hex.Rotation;
	FVector HexRotation = FVector(Hex.Rotation.Pitch, Hex.Rotation.Yaw, Hex.Rotation.Roll);
	float maxFlatDiviation = HexRotation.GetAbsMax();
	if (Hex.AttachedBuilding == nullptr && maxFlatDiviation <= 15.f)
	{
		return true;
	}
	else {
		return false;
	}
}


/******************** IsHexBuildable *************************/
APOTLStructure* UPOTLGameInstance::PlantStructure(FVector CubeCoord, FString RowName, FString TreeId, APOTLStructure* EmitTo)
{
	Log("PlantStructure", 15.0f, FColor::Yellow, -1);
	APOTLStructure* Structure = nullptr;

	if (Landscape)
	{
		Log("Landscape", 15.0f, FColor::Yellow, -1);
	}

	if (StructureTable)
	{
		Log("StructureTable", 15.0f, FColor::Yellow, -1);
	}

	//Log("PlantStructure : " + FString::FromInt(StructureTable->GetRowNames().Num()), 15.0f, FColor::Yellow, -1);
	 
	if (Landscape && StructureTable)
	{
		static const FString ContextString(TEXT("GENERAL")); //~~ Key value for each column of values ~~//
		FST_Structure* StructureData = StructureTable->FindRow<FST_Structure>(*RowName, ContextString);
		if (StructureData)
		{
			FVector2D OffsetCoords = UPOTLUtilFunctionLibrary::ConvertCubeToOffset(CubeCoord);
			int32 HexIndex = UPOTLUtilFunctionLibrary::GetHexIndex(OffsetCoords, GridXCount);
			if (Hexes.IsValidIndex(HexIndex))
			{
				FST_Hex& Hex = Hexes[HexIndex];
				UWorld* World = Landscape->GetWorld();
				if (World)
				{
					//~~ Set the spawn parameters ~~//
					FActorSpawnParameters SpawnParams;
					//SpawnParams.Owner = this;
					//SpawnParams.Instigator = Instigator;
					SpawnParams.bNoCollisionFail = true; //~~ Spawn event if collision ~~//

					FVector SpawnLocation = Hex.Location;
					FRotator SpawnRotation = Hex.Rotation;

					// Spawn the pickup
					//APOTLStructure* const
					Structure = World->SpawnActor<APOTLStructure>(StructureData->StructureClass, SpawnLocation, SpawnRotation, SpawnParams);
					
					Log("Spawn", 15.0f, FColor::Yellow, -1);

					if (TreeId != "" && TreeId != "None")
					{
						Structure->TreeId = TreeId;
					}
					else
					{
						Structure->TreeId = Structure->GetName();
					}

					//~~ Tree id logic here ~~//

					//~~ Set Structure on all hexes based on cube location and structure size ~~//
					for (int32 I = 0; I > StructureData->CubeSizes.Num(); I++)
					{
						FVector LocalCubeCoord = StructureData->CubeSizes[I] + CubeCoord;
						FVector2D OffsetCoords = UPOTLUtilFunctionLibrary::ConvertCubeToOffset(LocalCubeCoord);
						int32 HexIndex = UPOTLUtilFunctionLibrary::GetHexIndex(OffsetCoords, GridXCount);
						if (Hexes.IsValidIndex(HexIndex))
						{
							FST_Hex& Hex = Hexes[HexIndex];
							Hex.AttachedBuilding = Structure;
						}
					}
					//~~ Store hexindex in structure ~~//
					FVector2D OffsetCoords = UPOTLUtilFunctionLibrary::ConvertCubeToOffset(CubeCoord);
					int32 HexIndex = UPOTLUtilFunctionLibrary::GetHexIndex(OffsetCoords, GridXCount);
					if (Hexes.IsValidIndex(HexIndex))
					{
						Structure->HexIndex = HexIndex;
					}
					//~~ Create Broadcast/Emit Connection ~~//
					if (EmitTo)
					{
						CreateStructureConnection(Structure, EmitTo);
					}
				}
			}
		}
	}
	return Structure;
}

/******************** IsHexBuildable *************************/
void UPOTLGameInstance::CreateStructureConnection(APOTLStructure* From, APOTLStructure* To)
{
	From->EmitTo = To;
	From->TreeId = To->TreeId;
	To->BroadcastTo.Add(From);
}


/*****************************************************************************************************/
/**************************************** MAP - CREATION *********************************************/
/*****************************************************************************************************/

/******************** TraceLandscape *************************/
void UPOTLGameInstance::TraceLandscape(ECollisionChannel CollisionChannel)
{
	//UGameplayStatics::

	if (Landscape)
	{
		FVector ActorLocation = Landscape->GetActorLocation();
		// Get player controller at index 0
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(Landscape->GetWorld(), 0);
		if (PlayerController)
		{
			//UWorld *World = Landscape->GetWorld();
			//const FName TraceTag("MyTraceTag");
			//World->DebugDrawTraceTag = TraceTag;

			//Landscape->GetWorld()->DebugDrawTraceTag = TraceTag;

			FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, PlayerController);
			RV_TraceParams.bTraceComplex = true;
			RV_TraceParams.bTraceAsyncScene = true;
			RV_TraceParams.bReturnPhysicalMaterial = false;
			//RV_TraceParams.TraceTag = TraceTag;

			//ECollisionChannel CollisionChannel = ECC_Pawn;

			//Re-initialize hit info
			FHitResult RV_Hit(ForceInit);

			for (int32 Row = 0; Row <= GridYCount; Row++)
			{
				for (int32 Column = 0; Column <= GridXCount; Column++)
				{
					float X = Column * (HexWidth / 2);
					float Y = (Column + Row + 1) % 2 * 74 + (Row * 255) - (34 * Row); 
					FVector LineTraceFrom = ActorLocation + FVector{ X, Y, 3000 } +FVector{ 1.f, 1.f, 0.f };
					FVector LineTraceTo = ActorLocation + FVector{ X, Y, -3000 } +FVector{ 1.f, 1.f, 0.f };

					PlayerController->GetWorld()->LineTraceSingleByChannel(RV_Hit, LineTraceFrom, LineTraceTo, CollisionChannel, RV_TraceParams);
					//if (RV_Hit.GetActor() != NULL)
					if (RV_Hit.bBlockingHit)
					{
						FST_Point Point;
						Point.Location = RV_Hit.Location;
						Point.Row = Row;
						Point.Column = Column;
						Point.Exits = true;
						Points.Add(Point);
					}
				}
			}
		}
	}
}


/******************** CreateHexes *************************/
void UPOTLGameInstance::CreateHexes(ECollisionChannel CollisionChannel)
{
	if (Landscape != NULL) {
		// Get player controller at index 0
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(Landscape->GetWorld(), 0);

		if (PlayerController)
		{
			FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, PlayerController);
			RV_TraceParams.bTraceComplex = true;
			RV_TraceParams.bTraceAsyncScene = true;
			RV_TraceParams.bReturnPhysicalMaterial = false;

			// Reset hexes
			Hexes.Empty();

			//Re-initialize hit info
			FHitResult RV_Hit(ForceInit);

			float X = HexWidth / 2;
			float Y = HexWidth / FMath::Sqrt(3) * 2 / 4;

			for (int32 i = 0; i < Points.Num(); i++)
			{
				FST_Point& Point = Points[i];
				FVector LineTraceFrom = Point.Location + FVector{ X, Y, 3000 };
				FVector LineTraceTo = Point.Location + FVector{ X, Y, -3000 };

				int32 Creator = ((Point.Row + 1) % 2) + ((Point.Column + 2) % 2);
				if (Creator == 1)
				{
					Point.IsCreator = true;
					PlayerController->GetWorld()->LineTraceSingleByChannel(RV_Hit, LineTraceFrom, LineTraceTo, CollisionChannel, RV_TraceParams);
					if (RV_Hit.bBlockingHit)
					{
						FST_Hex Hex;
						Hex.Location = RV_Hit.Location;
						Hex.HexOffsetCoords = FVector2D{ (float)FMath::FloorToInt((float)Point.Column / 2), (float)FMath::FloorToInt((float)Point.Row) };
						Hex.HexCubeCoords = UPOTLUtilFunctionLibrary::ConvertOffsetToCube(Hex.HexOffsetCoords);

						// Points Ref
						int32 PointIndex = -1;
						Hex.Point0 = Point;
						
						PointIndex = UPOTLUtilFunctionLibrary::GetGridIndex(GridXCount, Point.Column + 1, Point.Row, true);
						
						if (Points.IsValidIndex(PointIndex))
						{
							Hex.Point1 = Points[PointIndex];
						}
						PointIndex = UPOTLUtilFunctionLibrary::GetGridIndex(GridXCount, Point.Column + 2, Point.Row, true);
						if (Points.IsValidIndex(PointIndex))
						{
							Hex.Point2 = Points[PointIndex];
						}
						PointIndex = UPOTLUtilFunctionLibrary::GetGridIndex(GridXCount, Point.Column + 2, Point.Row + 1, true);
						if (Points.IsValidIndex(PointIndex))
						{
							Hex.Point3 = Points[PointIndex];
						}
						PointIndex = UPOTLUtilFunctionLibrary::GetGridIndex(GridXCount, Point.Column + 1, Point.Row + 1, true);
						if (Points.IsValidIndex(PointIndex))
						{
							Hex.Point4 = Points[PointIndex];
						}
						PointIndex = UPOTLUtilFunctionLibrary::GetGridIndex(GridXCount, Point.Column, Point.Row + 1, true);
						if (Points.IsValidIndex(PointIndex))
						{
							Hex.Point5 = Points[PointIndex];
						}
						Hexes.Add(Hex);
					}
				}
			}
		}
	}
}


/******************** CleanHexes *************************/
void UPOTLGameInstance::CleanHexes()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "CleanHexes");
	TArray<FST_Hex> ValidHexes;
	int32 Count = 0;
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Hexes.Num() - before clean" + FString::FromInt(Hexes.Num()));
	for (int32 i = 0; i < Hexes.Num(); i++)
	{
		FST_Hex Hex = Hexes[i];
		bool Remove = (!Hex.Point0.Exits || !Hex.Point1.Exits || !Hex.Point2.Exits || !Hex.Point3.Exits || !Hex.Point4.Exits || !Hex.Point5.Exits);
		if (Remove)
		{
			Count++;
			Hex.Remove = true;
		}
		else
		{
			FString Msg = "Add Hex : ";
			Msg += Hex.Remove ? "true" : "false";
			ValidHexes.Add(Hex);
		}
	}
	FString CountMsg = FString::FromInt(Count);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, CountMsg);

	Hexes = ValidHexes;
}


/******************** EnrichHexes *************************/
void UPOTLGameInstance::EnrichHexes()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "EnrichHexes");
	int32 Count = 0;
	for (int32 i = 0; i < Hexes.Num(); i++)
	{
		FST_Hex& Hex = Hexes[i];
		Hex.HexIndex = i; // Set HexIndex

		TArray<FVector> CubeDirections;
		CubeDirections.Add({ 0, 1, -1 });
		CubeDirections.Add({ 1, 0, -1 });
		CubeDirections.Add({ 1, -1, 0 });
		CubeDirections.Add({ 0, -1, 1 });
		CubeDirections.Add({ -1, 0, 1 });
		CubeDirections.Add({ -1, 1, 0 });

		FVector CubeCoord = Hex.HexCubeCoords;
		for (int32 ii = 0; ii < CubeDirections.Num(); ii++)
		{
			FVector CubeDirection = CubeDirections[ii];
			FVector CombinedVector = CubeCoord + CubeDirection;
			
			FVector2D OffsetCoords = UPOTLUtilFunctionLibrary::ConvertCubeToOffset(CombinedVector);
			int32 HexDirectionIndex = UPOTLUtilFunctionLibrary::GetHexIndex(OffsetCoords, GridXCount);
			if (Hexes.IsValidIndex(HexDirectionIndex))
			{
				Hex.HexNeighborIndexes[ii] = HexDirectionIndex;
			}
		}
	}
}


/******************** CalcHexesRot *************************/
void UPOTLGameInstance::CalcHexesRot()
{
	float HexRealHeight = HexWidth / FMath::Sqrt(3) * 2;
	for (int32 i = 0; i < Hexes.Num(); i++)
	{
		FST_Hex& Hex = Hexes[i];
		FRotator Rotation;

		float Angle;
		float xDiff;
		float yDiff;

		xDiff = Hex.Point2.Location.X - Hex.Point0.Location.X;
		yDiff = Hex.Point2.Location.Z - Hex.Point0.Location.Z;
		Angle = FMath::Atan2(yDiff, xDiff) * (180 / 3.141592);
		Rotation.Pitch = Angle;

		Rotation.Yaw = 0.0f;

		xDiff = Hex.Point4.Location.Y - Hex.Point1.Location.Y;
		yDiff = Hex.Point4.Location.Z - Hex.Point1.Location.Z;
		Angle = FMath::Atan2(yDiff, xDiff) * (180 / 3.141592) * -1;
		Rotation.Roll = Angle;

		Hex.Rotation = Rotation;
	}
}

/*****************************************************************************************************/
/****************************************** Turn *****************************************************/
/*****************************************************************************************************/

/******************** SwitchTurn *************************/
void UPOTLGameInstance::SwitchTurn()
{
	OnTurnSwitched.Broadcast(32.f);
	//GetWorld()->GetTimerManager().SetTimer(this, &UPOTLGameInstance::NewTurn, 5.0f, false);
	//TimerManager->SetTimer(TurnTimerHandle, this, &UPOTLGameInstance::NewTurn, 1.0f, false);

	NewTurn(0.2f);
}

/******************** NewTurn *************************/
void UPOTLGameInstance::NewTurn(float WaitTime)
{
	if (Landscape)
	{
		bool AllResolved = true;
		for (int32 i = 0; i < Structures.Num(); i++)
		{
			APOTLStructure* Structure = Structures[i];
			if (!Structure->IsResolvedThisTurn)
			{
				AllResolved = false;
				break;
			}
		}
		if (AllResolved)
		{
			OnNewTurn.Broadcast(17.f);
		}
		else {
			FTimerHandle UniqueHandle;
			FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &UPOTLGameInstance::NewTurn, WaitTime * 2); //~~ Doubles wait time for each loop ~~//
			Landscape->GetWorldTimerManager().SetTimer(UniqueHandle, TimerDelegate, WaitTime, false);
		}
	}
}

/*****************************************************************************************************/
/**************************************** UTIL - Hex *************************************************/
/*****************************************************************************************************/

/******************** MouseToHex *************************/
FST_Hex UPOTLGameInstance::MouseToHex(ECollisionChannel CollisionChannel)
{
	FST_Hex Hex = FST_Hex{};
	if (Landscape)
	{
		FVector LandscapeLocation = Landscape->GetActorLocation();
		// Get player controller at index 0
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(Landscape->GetWorld(), 0);
		if (PlayerController)
		{
			FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, PlayerController);
			RV_TraceParams.bTraceComplex = true;
			RV_TraceParams.bTraceAsyncScene = true;
			RV_TraceParams.bReturnPhysicalMaterial = false;

			//Re-initialize hit info
			FHitResult RV_Hit(ForceInit);

			FVector WorldLocation;
			FVector WorldDirection;
			PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

			//FRotator currentCharacterRotation = this->GetActorRotation();
			//FRotator targetRotation = mouseDirection.Rotation();

			//WorldLocation = WorldLocation + LandscapeLocation;

			FVector LineTraceFrom = WorldLocation + FVector{ 1.f, 1.f, 0.f };
			FVector LineTraceTo = WorldDirection * 50000 + WorldLocation + FVector{ 1.f, 1.f, 0.f };

			PlayerController->GetWorld()->LineTraceSingleByChannel(RV_Hit, LineTraceFrom, LineTraceTo, CollisionChannel, RV_TraceParams);
			if (RV_Hit.bBlockingHit)
			{
				// Point.Location = RV_Hit.Location;
				Hex = LocationToHex(RV_Hit.Location);
			}
		}
	}
	return Hex;
}


/******************** LocationToHex *************************/
FST_Hex UPOTLGameInstance::LocationToHex(FVector Location)
{
	FST_Hex Hex = FST_Hex{};
	FVector Cube = UPOTLUtilFunctionLibrary::LocationToCube(GridXCount, HexWidth, HexHeight, Location);
	int32 HexIndex = UPOTLUtilFunctionLibrary::GetHexIndex(UPOTLUtilFunctionLibrary::ConvertCubeToOffset(Cube), GridXCount);
	if (Hexes.IsValidIndex(HexIndex)) {
		Hex = Hexes[HexIndex];
	}
	return Hex;
}

/*****************************************************************************************************/
/**************************************** DEBUG - LOG ************************************************/
/*****************************************************************************************************/

/******************** Log *************************/
void UPOTLGameInstance::Log(FString Msg = "", float Duration = 5.0f, FColor DebugColor = FColor::Green, int32 GroupIndex = -1)
{
	GEngine->AddOnScreenDebugMessage(GroupIndex, Duration, DebugColor, Msg);
}




/*****************************************************************************************************/
/**************************************** DELEGATES **************************************************/
/*****************************************************************************************************/

//OnTurnSwitched.AddDynamic(this);
// Broadcast Delegate 
//OnTurnSwitched.Broadcast(CurrentHouer, CurrentMinute, CurrentSeconds);


//Log("Structure->BroadcastRange: " + FString::FromInt(Structure->BroadcastRange), 15.0f, FColor::Yellow, 3);

//Log("k: " + FString::FromInt(k) + "/" + FString::FromInt(m), 15.0f, FColor::Yellow, -1);
//Log("hex cube: " + Hex.HexCubeCoords.ToString(), 15.0f, FColor::Yellow, -1);


/*
for (int32 testI = 0; testI < Structure->Hex.HexNeighborIndexes.Num(); testI++)
{
int32 Index = Structure->Hex.HexNeighborIndexes[testI];

//Log("testI: " + FString::FromInt(testI) + "  Index:" + FString::FromInt(Index) + FString::Printf(TEXT("Bool: %s"), (Index != -1 && Hexes.IsValidIndex(Index) ? TEXT("true") : TEXT("false"))), 15.0f, FColor::Green, -1);

if (Index != -1 && Hexes.IsValidIndex(Index))
{
FST_Hex& NeighborHex = Hexes[Index];

// Make Construct Location
FST_ConstructLocation ConstructLocation;
ConstructLocation.Cube = NeighborHex.HexCubeCoords;
ConstructLocation.Hex = NeighborHex;
ConstructLocation.EmitTo.Add(NeighborHex);
ConstructLocations.Add(ConstructLocation);
}
}
Log("ConstructLocations.Num(): " + FString::FromInt(ConstructLocations.Num()), 15.0f, FColor::Green, 4);
return ConstructLocations;
*/