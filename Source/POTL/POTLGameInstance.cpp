// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLStructure.h"
#include "POTLGameInstance.h"



// Sets default values
UPOTLGameInstance::UPOTLGameInstance(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{


}


/******************** GetConstructLocations *************************/
TArray<FST_Hex> UPOTLGameInstance::GetConstructLocations(APOTLStructure* Structure, bool IncludeChildren)
{
	//Log("GetConstructLocations", 15.0f, FColor::Yellow, 2);
	TArray<FST_Hex> ConstructHexes;
	TArray<int32> ConstructHexIndexes;
	ConstructHexIndexes = GetConstructLocationIndexes(Structure, IncludeChildren); // Get self hex indexes
	// Get children hex indexes
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
	// Convert indexes into real hexes and send a array of copies of them
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

	VisitedHexIndexes.Add(Structure->HexIndex); // Add Start Hex to VisitedHexes

	Frontier frontier;
	frontier.HexIndexes.Add(Structure->HexIndex);
	Frontiers.Add(frontier);

	FName TreeId = Structure->TreeId;

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
			// Reset construct info if the storred tre id ain't the same.
			if (Hex.ConstructInfo.TreeId != TreeId)
			{
				Hex.ConstructInfo = FST_ConstructLocation{};
			}
			// Make Construct Location
			Hex.ConstructInfo.Cube = Hex.HexCubeCoords;
			Hex.ConstructInfo.EmitTo.Add(Structure);  // Don't know if it should be a hex or structure reference to, for it to be the best solution.
			//Hex.ConstructInfo.EmitTo.Add(Hex);

			// Store broadcasted resources? NO. Will result in a lot of hex updating. Could then result in bugs with imcomplete data.


			// Add neighbors to the new frontier/next step. Only if they haven't been visited yet.
			for (int32 i = 0; i < Hex.HexNeighborIndexes.Num(); i++)
			{
				int32 Index = Hex.HexNeighborIndexes[i];
				if (Index != -1 && Hexes.IsValidIndex(Index))
				{
					FST_Hex& NeighborHex = Hexes[Index];
					if (NeighborHex.AttachedBuilding != nullptr // Only if pointer to structure isn't null
						&& !Hex.ConstructInfo.AttachTo.Contains(NeighborHex.AttachedBuilding) // Only if structure isn't already stored in attachments. Will cause structure to block for itself
						&& NeighborHex.AttachedBuilding->TreeId == TreeId) //  Only structures in same Tree
					{
						Hex.ConstructInfo.AttachTo.Add(NeighborHex.AttachedBuilding);
					}
					if (!VisitedHexIndexes.Contains(Index))
					{
						// Search for attachments/adjacent buildings and store them in hexes
						if (IsHexBuildable(NeighborHex))
						{
							Frontiers[k].HexIndexes.Add(Index); // Add Neighbor Hex to the next frontier
							VisitedHexIndexes.Add(Index); // Add index to visited indexes, so that neighbors don't overlap each other.
						}
					}
				}
			}
			Hex.DebugMe = true;
			//ConstructHexIndexes.Add(Hex.HexIndex);
			ConstructHexIndexes.AddUnique(Hex.HexIndex);
		}
	}
	Structure->BroadcastGridHexIndexes = ConstructHexIndexes; // Store indexes in structure. It cludes all hex broadcast grid index, including childrens hex indexes
	return ConstructHexIndexes; // Return
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


/******************** Log *************************/
void UPOTLGameInstance::Log(FString Msg = "", float Duration = 5.0f, FColor DebugColor = FColor::Green, int32 GroupIndex = -1)
{
	GEngine->AddOnScreenDebugMessage(GroupIndex, Duration, DebugColor, Msg);
}





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