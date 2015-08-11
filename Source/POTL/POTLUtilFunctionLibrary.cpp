// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include <string>
#include "POTLUtilFunctionLibrary.h"



// Sets default values
UPOTLUtilFunctionLibrary::UPOTLUtilFunctionLibrary(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}



FString UPOTLUtilFunctionLibrary::FindThisFunction(FString ReturnString)
{




	return ReturnString;
}


// Get Grid Index
int32 UPOTLUtilFunctionLibrary::GetGridIndex(int32 GridWidth, int32 Column, int32 Row, bool NoWrap)
{
	int32 index;
	float insideGrid;
	// -1 == Outside grid || 0 == Inside grid
	insideGrid = FMath::FloorToInt((Column + 1) / (GridWidth + 2)) * -1;
	index = ((Row * GridWidth) + Column + Row) * (1 + (2 * insideGrid));
	return index;
}


// Get Hex Index
int32 UPOTLUtilFunctionLibrary::GetHexIndex(FVector2D OffsetCoord, int32 GridXCount)
{
	int32 GridWidth = (GridXCount - (GridXCount % 2)) / 2 - 1;
	int32 Index = GetGridIndex(GridWidth, FMath::FloorToInt(OffsetCoord.X), FMath::FloorToInt(OffsetCoord.Y), true);
	Index = Index - (FMath::FloorToInt(OffsetCoord.Y) * ((GridXCount + 1 ) % 2));
	return Index;
}

bool UPOTLUtilFunctionLibrary::PointIndexValid(TArray<FST_Point> Points, int32 Index)
{
	bool valid = (Index < Points.Num()) && (Index >= 0);
	return valid;
}

/**--- AMIT ------------------------*/

int32 UPOTLUtilFunctionLibrary::GetCubeDistance(FVector CubeCoordsFrom, FVector CubeCoordsTo)
{
	int32 Distance;
	FVector DistanceVector = CubeCoordsFrom - CubeCoordsTo;
	Distance = DistanceVector.GetAbsMax();
	//float X = FMath::Max(DistanceVector.X);
	return Distance;
}

FVector UPOTLUtilFunctionLibrary::ConvertOffsetToCube(FVector2D OffsetCoords)
{
	FVector CubeCoords;
	CubeCoords.X = OffsetCoords.X - ((FMath::FloorToInt(OffsetCoords.Y) - (FMath::FloorToInt(OffsetCoords.Y) % 2)) / 2);
	CubeCoords.Y = (CubeCoords.X * -1) - OffsetCoords.Y;
	CubeCoords.Z = OffsetCoords.Y;
	return CubeCoords;
}

FVector2D UPOTLUtilFunctionLibrary::ConvertCubeToOffset(FVector CubeCoords)
{
	int32 CubeCoordsZFloored = FMath::FloorToInt(CubeCoords.Z);
	FVector2D OffsetCoords;
	OffsetCoords.X = (FMath::FloorToInt(CubeCoords.X) + ((CubeCoordsZFloored - CubeCoordsZFloored % 2) / 2));
	OffsetCoords.Y = CubeCoords.Z;
	return OffsetCoords;
}

FVector UPOTLUtilFunctionLibrary::AxialToCube(float Q, float R)
{
	FVector CubeCoords;
	CubeCoords.X = Q;
	CubeCoords.Y = (Q * -1) - R;
	CubeCoords.Z = R;
	return CubeCoords;
}


/** Map - Creation */

TArray<FST_Point> UPOTLUtilFunctionLibrary::TraceLandscape(AActor* Landscape, int32 GridXCount, int32 GridYCount, float HexWidth)
{
	//UGameplayStatics::
	TArray<FST_Point> Points;
	
	if (Landscape != NULL)
	{
		FVector ActorLocation = Landscape->GetActorLocation();
		// Get player controller at index 0
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(Landscape->GetWorld(), 0);
		if (PlayerController)
		{
			FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, PlayerController);
			RV_TraceParams.bTraceComplex = true;
			RV_TraceParams.bTraceAsyncScene = true;
			RV_TraceParams.bReturnPhysicalMaterial = false;

			ECollisionChannel CollisionChannel = ECC_Pawn;

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

					PlayerController->GetWorld()->LineTraceSingle(RV_Hit, LineTraceFrom, LineTraceTo, CollisionChannel, RV_TraceParams);
					if (RV_Hit.GetActor() != NULL)
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
	return Points;
}

TArray<FST_Hex> UPOTLUtilFunctionLibrary::CreateHexes(AActor* Landscape, TArray<FST_Point> Points, int32 GridXCount,  float HexWidth)
{
	TArray<FST_Hex> Hexes;
	
	if (Landscape != NULL) {
		// Get player controller at index 0
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(Landscape->GetWorld(), 0);

		if (PlayerController)
		{
			FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, PlayerController);
			RV_TraceParams.bTraceComplex = true;
			RV_TraceParams.bTraceAsyncScene = true;
			RV_TraceParams.bReturnPhysicalMaterial = false;

			ECollisionChannel CollisionChannel = ECC_Pawn;

			//Re-initialize hit info
			FHitResult RV_Hit(ForceInit);

			float X = HexWidth / 2;
			float Y = HexWidth / FMath::Sqrt(3) * 2 / 4;

			for (int32 i = 0; i < Points.Num(); i++)
			{
				FST_Point Point = Points[i];
				FVector LineTraceFrom = Point.Location + FVector{ X, Y, 3000 };
				FVector LineTraceTo = Point.Location + FVector{ X, Y, -3000 };

				int32 Creator = ((Point.Row + 1) % 2) + ((Point.Column + 2) % 2);
				if (Creator == 1)
				{
					Point.IsCreator = true;
					PlayerController->GetWorld()->LineTraceSingle(RV_Hit, LineTraceFrom, LineTraceTo, CollisionChannel, RV_TraceParams);
					if (RV_Hit.GetActor() != NULL)
					{
						FST_Hex Hex;
						Hex.Location = RV_Hit.Location;
						Hex.HexCubeCoords = FVector{ 0, 0, 0 };
						Hex.HexOffsetCoords = FVector2D{ (float)Point.Column / 2, (float)Point.Row };

						// Points Ref
						int PointIndex;
						Hex.Point0 = Point;

						PointIndex = GetGridIndex(GridXCount, Point.Column + 1, Point.Row, true);
						if (PointIndexValid(Points, PointIndex))
						{
							Hex.Point1 = Points[PointIndex];
						}
						PointIndex = GetGridIndex(GridXCount, Point.Column + 2, Point.Row, true);
						if (PointIndexValid(Points, PointIndex))
						{
							Hex.Point2 = Points[PointIndex];
						}
						PointIndex = GetGridIndex(GridXCount, Point.Column + 2, Point.Row + 1, true);
						if (PointIndexValid(Points, PointIndex))
						{
							Hex.Point3 = Points[PointIndex];
						}
						PointIndex = GetGridIndex(GridXCount, Point.Column + 1, Point.Row + 1, true);
						if (PointIndexValid(Points, PointIndex))
						{
							Hex.Point4 = Points[PointIndex];
						}
						PointIndex = GetGridIndex(GridXCount, Point.Column, Point.Row + 1, true);
						if (PointIndexValid(Points, PointIndex))
						{
							Hex.Point5 = Points[PointIndex];
						}

						/*
						Hex.Point1 = Points[GetGridIndex(GridXCount, Point.Column + 1, Point.Row, true)];
						Hex.Point2 = Points[GetGridIndex(GridXCount, Point.Column + 2, Point.Row, true)];
						Hex.Point3 = Points[GetGridIndex(GridXCount, Point.Column + 2, Point.Row + 1, true)];
						Hex.Point4 = Points[GetGridIndex(GridXCount, Point.Column + 1, Point.Row + 1, true)];
						Hex.Point5 = Points[GetGridIndex(GridXCount, Point.Column, Point.Row + 1, true)];
						*/

						Hexes.Add(Hex);
					}
				}
			}
		}
	}
	return Hexes;
}

TArray<FST_Hex> UPOTLUtilFunctionLibrary::CleanHexes(TArray<FST_Hex> Hexes)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "CleanHexes");
	TArray<FST_Hex> ValidHexes;
	int32 Count = 0;
	for (int32 i = 0; i < Hexes.Num(); i++)
	{
		FST_Hex Hex = Hexes[i];
		bool Remove = (!Hex.Point0.Exits || !Hex.Point1.Exits || !Hex.Point2.Exits || !Hex.Point3.Exits || !Hex.Point4.Exits || !Hex.Point5.Exits);
		if (Remove)
		{
			Count++;
			Hex.Remove = true;
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Remove Hex");
		}
		else 
		{
			FString Msg = "Add Hex : ";
			Msg += Hex.Remove ? "true" : "false";
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, Msg);
			ValidHexes.Add(Hex);
		}
	}
	FString CountMsg = FString::FromInt(Count);

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, CountMsg);
	return ValidHexes;
}

TArray<FST_Hex> UPOTLUtilFunctionLibrary::CalcHexesRot(TArray<FST_Hex> Hexes, float HexWidth)
{
	//TArray<FST_Hex> CalcedHexes;
	float HexRealHeight = HexWidth / FMath::Sqrt(3) * 2;
	for (int32 i = 0; i < Hexes.Num(); i++)
	{
		FST_Hex Hex = Hexes[i];
		FRotator Rotation;
		Rotation.Pitch = (HexWidth / (Hex.Point2.Location.Z - Hex.Point0.Location.Z)) / 100 * 45 / 100;
		Rotation.Yaw = 0.0f;
		Rotation.Roll = (HexRealHeight / (Hex.Point1.Location.Z - Hex.Point4.Location.Z)) / 100 * 45 / 100;
		Hex.Rotation = Rotation;
	}
	//return CalcedHexes;
	return Hexes;
}



/** -- PATCHING --------------------*/

TArray<FVector> UPOTLUtilFunctionLibrary::GetHexesWithFloodFill(FVector StartPosition, TArray<FVector> Obstacles, int32 Range)
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