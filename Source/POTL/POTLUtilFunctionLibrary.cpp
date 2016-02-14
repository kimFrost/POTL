// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "POTLDataHolder.h"
#include <string>
#include "POTLUtilFunctionLibrary.h"



// Sets default values
UPOTLUtilFunctionLibrary::UPOTLUtilFunctionLibrary(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

/******************** RotateCube *************************/
FVector UPOTLUtilFunctionLibrary::RotateCube(FVector CubeCoord, int32 Direction, FVector CenterCube)
{
	Direction = Direction % 6;
	FVector RotatedCube = CubeCoord;
	FVector CubeLocal = CubeCoord - CenterCube;
	switch (Direction)
	{
		case 0:
		{
			break;
		}
		case 1:
		{
			RotatedCube = FVector{ -CubeLocal.Z, -CubeLocal.X, -CubeLocal.Y } + CenterCube;
			break;
		}
		case 2:
		{
			RotatedCube = FVector{ CubeLocal.Y, CubeLocal.Z, CubeLocal.X } + CenterCube;
			break;
		}
		case 3:
		{
			RotatedCube = FVector{ -CubeLocal.X, -CubeLocal.Y, -CubeLocal.Z } + CenterCube;
			break;
		}
		case 4:
		{
			RotatedCube = FVector{ CubeLocal.Z, CubeLocal.X, CubeLocal.Y } + CenterCube;
			break;
		}
		case 5:
		{
			RotatedCube = FVector{ -CubeLocal.Y, -CubeLocal.Z, -CubeLocal.X } + CenterCube;
			break;
		}
	}
	return RotatedCube;
}

/******************** RotateCubes *************************/
TArray<FVector> UPOTLUtilFunctionLibrary::RotateCubes(TArray<FVector> CubeCoords, int32 Direction, FVector CenterCube)
{
	Direction = Direction % 6;
	TArray<FVector> RotatedCubes;
	for (int32 i = 0; i < CubeCoords.Num(); i++)
	{
		FVector CubeCoord = CubeCoords[i];
		FVector CubeLocal = CubeCoord - CenterCube;
		switch(Direction)
		{
			case 0:
			{
				RotatedCubes.Add(CubeCoord);
				break;
			}
			case 1:
			{
				CubeCoord = FVector{ -CubeLocal.Z, -CubeLocal.X, -CubeLocal.Y } + CenterCube;
				RotatedCubes.Add(CubeCoord);
				break;
			}
			case 2:
			{
				CubeCoord = FVector{ CubeLocal.Y, CubeLocal.Z, CubeLocal.X } + CenterCube;
				RotatedCubes.Add(CubeCoord);
				break;
			}
			case 3:
			{
				CubeCoord = FVector{ -CubeLocal.X, -CubeLocal.Y, -CubeLocal.Z } + CenterCube;
				RotatedCubes.Add(CubeCoord);
				break;
			}
			case 4:
			{
				CubeCoord = FVector{ CubeLocal.Z, CubeLocal.X, CubeLocal.Y } + CenterCube;
				RotatedCubes.Add(CubeCoord);
				break;
			}
			case 5:
			{
				CubeCoord = FVector{ -CubeLocal.Y, -CubeLocal.Z, -CubeLocal.X } + CenterCube;
				RotatedCubes.Add(CubeCoord);
				break;
			}
		}
	}
	return RotatedCubes;
}


/******************** RoundCube *************************/
FVector UPOTLUtilFunctionLibrary::RoundCube(FVector Cube)
{	
	FVector RoundedCube;
	int32 Rx = FMath::RoundToInt(Cube.X);
	int32 Ry = FMath::RoundToInt(Cube.Y);
	int32 Rz = FMath::RoundToInt(Cube.Z);

	float XDiff = FMath::Abs(Rx - Cube.X);
	float YDiff = FMath::Abs(Ry - Cube.Y);
	float ZDiff = FMath::Abs(Rz - Cube.Z);

	if (XDiff > YDiff && XDiff > ZDiff)
	{
		Rx = -Ry - Rz;
	}
	else if (YDiff > ZDiff)
	{
		Ry = -Rx - Rz;
	}
	else
	{
		Rz = -Rx - Ry;
	}

	RoundedCube = FVector{ (float)(Rx), (float)(Ry), (float)(Rz) };
	return RoundedCube;
}


/******************** LocationToCube *************************/
FVector UPOTLUtilFunctionLibrary::LocationToCube(int32 GridXCount, float HexWidth, float HexHeight, FVector Location)
{
	Location = Location - FVector{ HexWidth / 2, HexHeight / 2, 0 };

	float Q = (Location.X * FMath::Sqrt(3) / 3 - Location.Y / 3) / (HexHeight / 2);
	float R = Location.Y * 2 / 3 / (HexHeight / 2);

	//float Temp = FMath::FloorToFloat(Location.X + FMath::Sqrt(3) * Location.Y + 1);
	//float Q = FMath::FloorToFloat((FMath::FloorToFloat(2 * Location.X + 1) + Temp) / 3);
	//float R = FMath::FloorToFloat((Temp + FMath::FloorToFloat(-Location.X + FMath::Sqrt(3) * Location.Y + 1)) / 3);

	LogMsg("Q: " + FString::FromInt(Q) + ", R: " + FString::FromInt(R), 15.0f, FColor::Yellow, 1);
	
	return RoundCube(AxialToCube(Q, R));
	//return RoundCube(ConvertOffsetToCube(FVector2D{ Q, R }));
}


/******************** GetGridIndex *************************/
int32 UPOTLUtilFunctionLibrary::GetGridIndex(int32 GridWidth, int32 Column, int32 Row, bool NoWrap)
{
	int32 index;
	float insideGrid;
	// -1 == Outside grid || 0 == Inside grid
	insideGrid = FMath::FloorToInt((Column + 1) / (GridWidth + 2)) * -1;
	index = ((Row * GridWidth) + Column + Row) * (1 + (2 * insideGrid));
	return index;
}


/******************** GetHexIndex *************************/
int32 UPOTLUtilFunctionLibrary::GetHexIndex(FVector2D OffsetCoord, int32 GridXCount)
{
	int32 GridWidth = (GridXCount - (GridXCount % 2)) / 2 - 1;
	int32 Index = GetGridIndex(GridWidth, FMath::FloorToInt(OffsetCoord.X), FMath::FloorToInt(OffsetCoord.Y), true);
	Index = Index - (FMath::FloorToInt(OffsetCoord.Y) / 2 * ((GridXCount + 1) % 2));
	return Index;
}


/******************** CubesToHexIndexes *************************/
TArray<int32> UPOTLUtilFunctionLibrary::CubesToHexIndexes(TArray<FVector> Cubes, int32 GridXCount)
{
	TArray<int32> HexIndexes;
	for (int32 i = 0; i < Cubes.Num(); i++)
	{
		FVector& Cube = Cubes[i];
		FVector2D Axial = ConvertCubeToOffset(Cube);
		int32 HexIndex = GetHexIndex(Axial, GridXCount);
		HexIndexes.Add(HexIndex);
	}
	return HexIndexes;
}


/******************** LogMsg *************************/
void UPOTLUtilFunctionLibrary::LogMsg(FString Msg = "", float Duration = 5.0f, FColor DebugColor = FColor::Green, int32 GroupIndex = -1)
{
	GEngine->AddOnScreenDebugMessage(GroupIndex, Duration, DebugColor, Msg);
}



/**--- AMIT ------------------------*/


/******************** GetCubeDistance *************************/
int32 UPOTLUtilFunctionLibrary::GetCubeDistance(FVector CubeCoordsFrom, FVector CubeCoordsTo)
{
	int32 Distance;
	FVector DistanceVector = CubeCoordsFrom - CubeCoordsTo;
	Distance = DistanceVector.GetAbsMax();
	//float X = FMath::Max(DistanceVector.X);
	return Distance;
}


/******************** GetCubesInRange *************************/
TArray<FVector> UPOTLUtilFunctionLibrary::GetCubesInRange(FVector CubeCoordsFrom, int32 Range, bool IncludeFrom)
{
	TArray<FVector> Cubes;
	if (Range > 0)
	{
		int32 NegativeRange = Range * -1;
		for (int32 Dx = NegativeRange; Dx <= Range; Dx++)
		{
			//int32 First = FMath::FloorToInt(FMath::Max3(0, 0, 0));
			int32 From = FMath::FloorToInt(FVector(NegativeRange, (Dx * -1) - Range, -99999999).GetMax());
			int32 To = FMath::FloorToInt(FVector(Range, (Dx * -1) + Range, 99999999).GetMin());
			for (int32 Dy = From; Dy <= To; Dy++)
			{
				int32 Dz = (Dx * -1) - Dy;
				Cubes.Add(FVector(Dx + CubeCoordsFrom.X, Dy + CubeCoordsFrom.Y, Dz + CubeCoordsFrom.Z));
			}
		}
	}
	if (!IncludeFrom)
	{
		Cubes.Remove(CubeCoordsFrom);
	}
	return Cubes;
}


/******************** ConvertOffsetToCube *************************/
FVector UPOTLUtilFunctionLibrary::ConvertOffsetToCube(FVector2D OffsetCoords)
{
	FVector CubeCoords;
	//CubeCoords.X = OffsetCoords.X - ((FMath::FloorToInt(OffsetCoords.Y) - (FMath::FloorToInt(OffsetCoords.Y) % 2)) / 2);
	//CubeCoords.Y = (CubeCoords.X * -1) - OffsetCoords.Y;
	//CubeCoords.Z = OffsetCoords.Y;
	CubeCoords.X = OffsetCoords.X - (OffsetCoords.Y - (FMath::FloorToInt(OffsetCoords.Y) % 2)) / 2;
	CubeCoords.Y = -CubeCoords.X - OffsetCoords.Y;
	CubeCoords.Z = OffsetCoords.Y;
	return CubeCoords;
}


/******************** ConvertCubeToOffset *************************/
FVector2D UPOTLUtilFunctionLibrary::ConvertCubeToOffset(FVector CubeCoords)
{
	int32 CubeCoordsZFloored = FMath::FloorToInt(CubeCoords.Z);
	FVector2D OffsetCoords;
	OffsetCoords.X = (FMath::FloorToInt(CubeCoords.X) + ((CubeCoordsZFloored - CubeCoordsZFloored % 2) / 2));
	OffsetCoords.Y = CubeCoords.Z;
	return OffsetCoords;
}


/******************** AxialToCube *************************/
FVector UPOTLUtilFunctionLibrary::AxialToCube(float Q, float R)
{
	FVector CubeCoords;
	CubeCoords.X = Q;
	CubeCoords.Y = (Q * -1) - R;
	CubeCoords.Z = R;
	return CubeCoords;
}


/******************** ActorExits *************************/
void UPOTLUtilFunctionLibrary::ActorExits(AActor* Actor, TEnumAsByte<EBoolGateEnum>& Branches)
{
	if (Actor)
	{
		Branches = EBoolGateEnum::Exits;
	}
	else
	{
		Branches = EBoolGateEnum::Null;
	}
}


/******************** HexesToHexIndex *************************/
TArray<int32> UPOTLUtilFunctionLibrary::HexesToHexIndexes(const TArray<FST_Hex>& Hexes)
{
	TArray<int32> HexIndexes;
	for (int32 i = 0; i < Hexes.Num(); i++)
	{
		HexIndexes.Add(Hexes[i].HexIndex);
	}
	return HexIndexes;
}


/******************** SubtractHexes *************************/
TArray<FST_Hex> UPOTLUtilFunctionLibrary::SubtractHexes(const TArray<FST_Hex>& Hexes, const TArray<FST_Hex>& Subtraction)
{
	TArray<FST_Hex> Result;
	TArray<int32> HexIndexes = HexesToHexIndexes(Hexes);
	TArray<int32> SubtractionIndexes = HexesToHexIndexes(Subtraction);
	int32 i;
	for (i = 0; i < SubtractionIndexes.Num(); i++)
	{
		HexIndexes.Remove(SubtractionIndexes[i]);
	}
	for (i = 0; i < Hexes.Num(); i++) 
	{ 
		if (HexIndexes.Contains(Hexes[i].HexIndex))
		{
			Result.Add(Hexes[i]);
		}
	}
	return Result;
}


/******************** IntersectHexes *************************/
TArray<FST_Hex> UPOTLUtilFunctionLibrary::IntersectHexes(const TArray<FST_Hex>& Hexes, const TArray<FST_Hex>& Intersection)
{
	TArray<FST_Hex> Result;
	TArray<int32> HexIndexes = HexesToHexIndexes(Hexes);
	TArray<int32> IntersectionIndexes = HexesToHexIndexes(Intersection);
	int32 i;
	for (i = HexIndexes.Num() - 1; i >= 0; i--)
	{
		if (!IntersectionIndexes.Contains(HexIndexes[i]))
		{
			HexIndexes.RemoveAt(i);
		}
	}
	for (i = 0; i < Hexes.Num(); i++)
	{
		if (HexIndexes.Contains(Hexes[i].HexIndex))
		{
			Result.Add(Hexes[i]);
		}
	}
	return Result;
}



/** -- MAP - PATHING --------------------*/


/******************** GetHexesWithFloodFill *************************/
TArray<FVector> UPOTLUtilFunctionLibrary::GetHexesWithFloodFill(FVector StartPosition, const TArray<FVector>& Obstacles, int32 Range)
{
	struct Fridge
	{
		TArray<FVector> CubeCoords;
	};

	TArray<FVector> VisitedCubeCoords;
	TArray<Fridge> Fridges;

	TArray<FVector> CubeDirections;

	CubeDirections.Add({ 1, 0, -1 });
	CubeDirections.Add({ 1, -1, 0 });
	CubeDirections.Add({ 0, -1, 1 });
	CubeDirections.Add({ -1, 0, 1 });
	CubeDirections.Add({ -1, 1, 0 });
	CubeDirections.Add({ 0, 1, -1 });

	// Add StartPosition to cubecoords already visited 
	VisitedCubeCoords.Add(StartPosition);

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
}


