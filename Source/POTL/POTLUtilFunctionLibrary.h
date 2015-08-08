// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "POTLUtilFunctionLibrary.generated.h"

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

	UFUNCTION(BlueprintCallable, Category = "Map")
	static void TraceLandscape();


};
