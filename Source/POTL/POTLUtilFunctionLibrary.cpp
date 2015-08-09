// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLUtilFunctionLibrary.h"



// Sets default values
UPOTLUtilFunctionLibrary::UPOTLUtilFunctionLibrary(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}



FString UPOTLUtilFunctionLibrary::FindThisFunction(FString ReturnString)
{




	return ReturnString;
}


void UPOTLUtilFunctionLibrary::TraceLandscape()
{

}


int32 UPOTLUtilFunctionLibrary::GetGridIndex(int32 GridWidth, int32 Column, int32 Row, bool NoWrap)
{
	int32 index;
	float insideGrid;

	// -1 == Outside grid || 0 == Inside grid
	insideGrid = FMath::Floor((Column + 1) / (GridWidth + 2)) * -1;
	index = ((Row * GridWidth) + Column + Row) * (1 + (2 * insideGrid));

	return index;
}


TArray<FVector> UPOTLUtilFunctionLibrary::GetHexesWithFloodFill(FVector StartPosition, TArray<FVector> Obstacles, int32 Range)
{
	struct Fridge
	{
		TArray<FVector> CubeCoords;
	};

	TArray<FVector> VisitedCubeCoords;
	//TArray<Fridge> Fridges;
	//FVector Fridges[100][6];
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

		/*
		TArray<FVector> CubeCoords = fridge.CubeCoords;
		for each (CubeCoord var in CubeCoords)
		{

		}
		*/
	}

	





	// Add StartPosition to cubecoords already visited 
	//VisitedCubeCoords.Add(StartPosition);
	//Fridges[0] = FVector[];

	//Fridge fridge = Fridge{0, 0, 0};
	//Fridges.Add(fridge);


	/*
	Fridges[0][0] = FVector{ StartPosition };

	for (int32 k = 1; k < Range; k++)
	{
		//Fridges[0][0] = FVector[];
		
		//FVector asdasasd[] = Fridges[0];

		for (int32 k = 1; k < Range; k++)
		{

		}
	}
	*/


	//auto array = new double[5][5];
	//int **ary = new int*[];



	return VisitedCubeCoords;
}