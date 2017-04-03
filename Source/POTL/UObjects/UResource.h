// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "UResource.generated.h"

//~~~~~ Forward Declarations ~~~~~//
class UStorageComponent;
class APOTLStructure;


UCLASS(Blueprintable, BlueprintType)
class POTL_API UResource : public UObject
{
	GENERATED_BODY()

public:
	UResource();
	~UResource();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	FString ResourceId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	TArray<FString> Tags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	float Value;

	bool Locked;

	float ExpirationTime;

	float LuxucyRating; // 1-10 factor. The richer the more luxury resource people will require.

	UStorageComponent* StoredIn;


	APOTLStructure* GetOwner();

	void Consume(bool bRemoveFromStorage = true);

	void RemoveFromStorage();

	bool Transfer(UStorageComponent* Storage);

	void Init();


	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	//int Amount;
	// No Amount. A resource is one resource



	/*
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	TArray<FString> Tags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	float UnitMass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	bool Stackable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	bool Burnable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	int32 StackSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	int32 Quantity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	int32 MaxAge;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	int32 SortSequence;
	*/

	
};
