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


private:

	TMap<FVector, UStorageComponent*> Storages;

	TArray<UResource*> Resources;

	TMap<FVector, int> QuantityMapOfType;


public:

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Storage")

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Storage")
	//TArray<UStorageComponent*> Storage;

	void IncludeStorage(UStorageComponent* StorageComp);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Storage")
	void StorageMapUpdated(UStorageComponent* StorageComp, UResource* Resource);

	UFUNCTION(BlueprintCallable, Category = "Storage")
	TArray<UResource*> GetResourceList();


	UResource* RequestResource(APOTLStructure* Requester, FString ResourceId);

	UResource* RequestResourceByTag(APOTLStructure* Requester, FString Tag);

	bool HasResourceAvailable(TArray<FString> ResourceIds);

	TArray<UResource*> RequestResources(APOTLStructure* Requester, TArray<FString> ResourceIds);

	UPROPERTY(BlueprintAssignable, Category = "Storage|Event")
	FOnStorageMapUpdated OnStorageMapUpdated;


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
