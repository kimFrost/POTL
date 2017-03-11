// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "UTransaction.generated.h"


//~~~~~ Forward Declarations ~~~~~//
class APOTLStructure;
class UResource;



UCLASS(Blueprintable, BlueprintType)
class POTL_API UTransaction : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transaction")
	APOTLStructure* Seller;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transaction")
	APOTLStructure* Buyer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transaction")
	UResource* Resource;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transaction")
	bool bConsume;


	// Transaction between house and vendor

	// House goes to vendor to buy bread


	// Resource / Item

	// Buyer 

	// Seller

	// Amount

	// Value

	// Timestamp

	// 


	// Resource takes x time per hex. Travel time to move

	
};
