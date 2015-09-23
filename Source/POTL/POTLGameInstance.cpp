// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLStructure.h"
#include "POTLGameInstance.h"



// Sets default values
UPOTLGameInstance::UPOTLGameInstance(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{


}


TArray<FST_ConstructLocation> UPOTLGameInstance::GetConstructLocations(APOTLStructure* Structure, bool IncludeChildren)
{
	Log("GetConstructLocations", 15.0f, FColor::Yellow, 2);

	TArray<FST_ConstructLocation> ConstructLocations;

	TArray<int32> VisitedHexIndexes;
	struct Frontier
	{
		TArray<FST_Hex> Hexes;
	};
	TArray<Frontier> Frontiers;

	// Add Start Hex to VisitedHexes
	VisitedHexIndexes.Add(Structure->Hex.HexIndex);

	Frontier frontier;
	frontier.Hexes.Add(Structure->Hex);
	Frontiers.Add(frontier);

	Log("Structure->BroadcastRange: " + FString::FromInt(Structure->BroadcastRange), 15.0f, FColor::Yellow, 3);

	for (int32 k = 1; k <= Structure->BroadcastRange; k++)
	{
		Frontier frontier;
		Frontiers.Add(frontier);

		frontier = Frontiers[k - 1];

		for (int32 m = 0; m < frontier.Hexes.Num(); m++)
		{

			FST_Hex& Hex = frontier.Hexes[m];

			//Log("k: " + FString::FromInt(k) + "/" + FString::FromInt(m), 15.0f, FColor::Yellow, -1);
			//Log("hex cube: " + Hex.HexCubeCoords.ToString(), 15.0f, FColor::Yellow, -1);

			for (int32 i = 0; i < Hex.HexNeighborIndexes.Num(); i++)
			{
				int32 Index = Hex.HexNeighborIndexes[i];
				if (Index != -1 && Hexes.IsValidIndex(Index))
				{
					FST_Hex& NeighborHex = Hexes[Index];
					if (!VisitedHexIndexes.Contains(Index) && IsHexBuildable(NeighborHex))
					{
						//VisitedHexIndexes.Add(Index); // Add to visited Indexes
						Frontiers[k].Hexes.Add(NeighborHex);

						// Make Construct Location
						FST_ConstructLocation ConstructLocation;
						ConstructLocation.Cube = Hex.HexCubeCoords;
						ConstructLocation.Hex = Hex;
						//ConstructLocation.EmitTo.Add(Structure); // Don't know if it should be a hex or structure reference to, for it to be the best solution.
						ConstructLocation.EmitTo.Add(Hex);
						ConstructLocations.Add(ConstructLocation);
					}
					VisitedHexIndexes.Add(Index); // Add to visited Indexes
				}
			}
		}

	}

	/*
	Fridge fridge;
	fridge.CubeCoords.Add(StartPosition);
	Fridges.Add(fridge);

	for (int32 k = 1; k <= Range; k++)
	{
		Fridge fridge;
		Fridges.Add(fridge);

		fridge = Fridges[k - 1];

		int arrayLength = fridge.CubeCoords.Num();
		for (int32 m = 0; m < arrayLength; m++)
		{
			FVector CubeCoord = fridge.CubeCoords[m];
			for (int32 l = 0; l < CubeDirections.Num(); l++)
			{
				FVector CubeDirection = CubeDirections[l];
				FVector CombinedVector = CubeCoord + CubeDirection;
				bool ExistsInVisited = VisitedCubeCoords.Contains(CombinedVector);
				bool ExistsInObstacles = Obstacles.Contains(CombinedVector);
				if (ExistsInVisited || ExistsInObstacles)
				{

				}
				else
				{
					VisitedCubeCoords.Add(CombinedVector);
					Fridges[k].CubeCoords.Add(CombinedVector);
				}
			}
		}
	}
	return VisitedCubeCoords;
	*/


	Log("ConstructLocations.Num(): " + FString::FromInt(ConstructLocations.Num()), 15.0f, FColor::Yellow, 4);

	if (IncludeChildren) {
		for (int32 i = 0; i < Structure->BroadcastTo.Num(); i++)
		{
			//TArray<FST_ConstructLocation> ChildrenConstructLocations = GetConstructLocations(Structure->BroadcastTo[i], true);
		}
	}

	return ConstructLocations;
}


bool UPOTLGameInstance::IsHexBuildable(FST_Hex& Hex)
{
	if (Hex.AttachedBuilding == NULL)
	{
		return true;
	}
	else {
		return false;
	}
}

void UPOTLGameInstance::Log(FString Msg = "", float Duration = 5.0f, FColor DebugColor = FColor::Green, int32 GroupIndex = -1)
{
	GEngine->AddOnScreenDebugMessage(GroupIndex, Duration, DebugColor, Msg);
}