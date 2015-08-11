// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "POTLUtilFunctionLibrary.generated.h"


USTRUCT(BlueprintType)
struct FST_Point
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Point")
	FVector Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Point")
	int32 Column;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Point")
	int32 Row;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Point")
	bool IsCreator = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Point")
	bool Exits = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Point")
	int32 Hex0Index = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Point")
	int32 Hex1Index = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Point")
	int32 Hex2Index = -1;
};


USTRUCT(BlueprintType)
struct FST_Hex
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hex")
	FVector Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FRotator Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FST_Point Point0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FST_Point Point1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FST_Point Point2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FST_Point Point3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FST_Point Point4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FST_Point Point5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	bool Remove = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	AActor* AttachedBuilding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	AActor* AttachedActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	AActor* AttachedMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FVector HexCubeCoords;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FVector2D HexOffsetCoords;

	/*
	FVector GetVector(int32 someInt)
	{
		FVector Test;
		return Test;
	}
	*/
};



UCLASS()
class POTL_API UPOTLUtilFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UPOTLUtilFunctionLibrary(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION(BlueprintCallable, Category="Hex")
	static FString FindThisFunction(FString ReturnString);

	UFUNCTION(BlueprintCallable, Category = "Hex")
	static TArray<FVector> GetHexesWithFloodFill(FVector StartPosition, TArray<FVector> Obstacles, int32 Range);

	

	/** AMIT */

	UFUNCTION(BlueprintCallable, Category = "Util")
	static int32 GetCubeDistance(FVector CubeCoordsFrom, FVector CubeCoordsTo);

	UFUNCTION(BlueprintCallable, Category = "Util")
	static FVector ConvertOffsetToCube(FVector2D OffsetCoords);

	UFUNCTION(BlueprintCallable, Category = "Util")
	static FVector2D ConvertCubeToOffset(FVector CubeCoords);

	UFUNCTION(BlueprintCallable, Category = "Util")
	static FVector AxialToCube(float Q, float R);



	/** Macro */

	UFUNCTION(BlueprintCallable, Category = "Util")
	static int32 GetGridIndex(int32 GridWidth, int32 Column, int32 Row, bool NoWrap);

	UFUNCTION(BlueprintCallable, Category = "Util")
	static int32 GetHexIndex(FVector2D OffsetCoord, int32 GridXCount);

	UFUNCTION(BlueprintCallable, Category = "Util")
	static bool PointIndexValid(TArray<FST_Point> Points, int32 Index);

	/** Map - Creation */

	UFUNCTION(BlueprintCallable, Category = "Map")
	static TArray<FST_Point> TraceLandscape(AActor* Landscape, int32 GridXCount, int32 GridCount, float HexWidth);

	UFUNCTION(BlueprintCallable, Category = "Map")
	static TArray<FST_Hex> CreateHexes(AActor* Landscape, TArray<FST_Point> Points, int32 GridXCount, float HexWidth);

	UFUNCTION(BlueprintCallable, Category = "Map")
	static TArray<FST_Hex> CleanHexes(TArray<FST_Hex> Hexes);

	UFUNCTION(BlueprintCallable, Category = "Map")
	static TArray<FST_Hex> CalcHexesRot(TArray<FST_Hex> Hexes,  float HexWidth);


};
