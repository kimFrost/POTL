// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLStructure.h"
#include "POTLGameInstance.h"



// Sets default values
UPOTLGameInstance::UPOTLGameInstance(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{


}


TArray<FST_Hex> UPOTLGameInstance::GetConstructLocations(APOTLStructure* Structure, bool IncludeChildren)
{
	Log("GetConstructLocations", 15.0f, FColor::Yellow, 2);

	//TArray<FST_ConstructLocation> ConstructLocations;
	TArray<FST_Hex> ConstructHexes;

	

	TArray<int32> VisitedHexIndexes;
	struct Frontier
	{
		TArray<FST_Hex> Hexes;
		//TArray<int32> HexIndexes;
	};
	TArray<Frontier> Frontiers;

	// Add Start Hex to VisitedHexes
	//VisitedHexIndexes.Add(Structure->Hex.HexIndex);
	VisitedHexIndexes.Add(Structure->HexIndex);

	
	

	
	Frontier frontier;
	//frontier.Hexes.Add(Structure->Hex);
	frontier.Hexes.Add(Hexes[Structure->HexIndex]);
	//frontier.HexIndexes.Add(Structure->HexIndex);
	Frontiers.Add(frontier);


	//FST_Hex& RealHex = Hexes[Structure->HexIndex];
	//RealHex.DebugMe = true;
	//Structure->Hex.DebugMe = true;

	FName TreeId = Structure->TreeId;

	//Hexes[Structure->HexIndex].DebugMe = true; // the hex stored in the structure is now no longer up to date
	//ConstructHexes.Add(Frontiers[0].Hexes[0]);

	

	//for (int32 k = 1; k <= Structure->BroadcastRange; k++)
	for (int32 k = 1; k <= 3; k++)
	{
		Frontier frontier;
		Frontiers.Add(frontier);
		frontier = Frontiers[k - 1];
		for (int32 m = 0; m < frontier.Hexes.Num(); m++)
		{
			FST_Hex& Hex = frontier.Hexes[m];
			// Reset construct info if the storred tre id ain't the same.
			if (Hex.ConstructInfo.TreeId != TreeId) 
			{
				Hex.ConstructInfo = FST_ConstructLocation{};
			}
			// Make Construct Location
			Hex.ConstructInfo.Cube = Hex.HexCubeCoords;
			Hex.ConstructInfo.EmitTo.Add(Structure);  // Don't know if it should be a hex or structure reference to, for it to be the best solution.
			//Hex.ConstructInfo.EmitTo.Add(Hex);
			
			// Add neighbors to the new frontier/next step. Only if they haven't been visited yet.
			for (int32 i = 0; i < Hex.HexNeighborIndexes.Num(); i++)
			{
				int32 Index = Hex.HexNeighborIndexes[i];
				if (Index != -1 && Hexes.IsValidIndex(Index))
				{
					FST_Hex& NeighborHex = Hexes[Index];
					// Make sure that the neighbor structure isn't null && the structure isn't alredy stored in attachTo
					if (NeighborHex.AttachedBuilding != nullptr && !Hex.ConstructInfo.AttachTo.Contains(NeighborHex.AttachedBuilding))
					{
						Hex.ConstructInfo.AttachTo.Add(NeighborHex.AttachedBuilding);
					}
					if (!VisitedHexIndexes.Contains(Index))
					{
						// Search for attachments/adjacent buildings and store them in hexes
						//NeighborHex.ConstructInfo.AttachTo.Add(Structure);
						if (IsHexBuildable(NeighborHex))
						{
							Frontiers[k].Hexes.Add(NeighborHex); // Add Neighbor Hex to the next frontier
							VisitedHexIndexes.Add(Index); // Add index to visited indexes, so that neighbors don't overlap each other.
						}
					}
				}
			}
			//Hex.ConstructLocations.Add(ConstructLocation);
			//Hex.DebugMe = true;
			ConstructHexes.Add(Hex);
		}
	}

	
	

	Log("ConstructHexes.Num(): " + FString::FromInt(ConstructHexes.Num()), 15.0f, FColor::Yellow, 4);

	if (IncludeChildren) {
		for (int32 i = 0; i < Structure->BroadcastTo.Num(); i++)
		{
			//TArray<FST_ConstructLocation> ChildrenConstructLocations = GetConstructLocations(Structure->BroadcastTo[i], true);
		}
	}

	return ConstructHexes;
}


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