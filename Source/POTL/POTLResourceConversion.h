// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataTable.h"
#include "POTLResourceConversion.generated.h"


USTRUCT(BlueprintType)
struct FST_ResourceConversion : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	FST_ResourceConversion()
		: Wood(0.f)
		, Planks(0.f)
	{}
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	float Wood;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	float Planks;
};


USTRUCT(BlueprintType)
struct FST_Recipe
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	FName Product;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	int32 Quantity;

	FST_Recipe()
	{
	}
};

USTRUCT(BlueprintType)
struct FST_ResourceRecipe : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	FST_ResourceRecipe()
		//: Wood(0.f)
		//, Planks(0.f)
	{}
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	//FName Product;
	UPROPERTY(EditAnywhere, Category = "Resources")
	TArray<FST_Recipe> Recipes;
	//TMap<FName, int32> Recipes;
};


/**
 * 
 */
UCLASS()
class POTL_API UPOTLResourceConversion : public UDataTable
{
	GENERATED_BODY()
	
	
	

};
