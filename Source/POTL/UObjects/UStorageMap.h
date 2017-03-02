// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "UObjects/UResource.h"
#include "UStorageMap.generated.h"

class UStorageComponent;

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStorageMapUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStorageMapUpdated, UStorageComponent*, StorageComp, UResource*, Resource);


UCLASS(Blueprintable, BlueprintType)
class POTL_API UStorageMap : public UObject
{
	GENERATED_BODY()

public:
	UStorageMap();
	~UStorageMap();

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Storage")
	TMap<FVector, UStorageComponent*> Storages;

	void IncludeStorage(UStorageComponent* StorageComp);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Storage")
	void StorageMapUpdated(UStorageComponent* StorageComp, UResource* Resource);

	UResource* RequestResource(APOTLStructure* Requester, FString ResourceId);

	UPROPERTY(BlueprintAssignable, Category = "Storage|Event")
	FOnStorageMapUpdated OnStorageMapUpdated;

private:

	TMap<FVector, int> QuantityMapOfType;

	//void OnStorageUpdate(UStorageComponent* StorageComp); // Bind to onstorageupdate and call this with storage and changes

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