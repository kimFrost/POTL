// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "UResource.generated.h"


//~~~~~ Forward Declarations ~~~~~//
class UStorageComponent;
class UProviderComponent;
class APOTLStructure;
class UHexTile;

//~~~~~ Enum ~~~~~//
UENUM(BlueprintType)
enum class EConsumeType : uint8
{
	Undefined UMETA(DisplayName = "Undefined"),
	Need UMETA(DisplayName = "Need"),
	Construction UMETA(DisplayName = "Construction")
};

//~~~~~ Delegates/Event dispatcher ~~~~~//
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FResourceOnAllocated, UResource*, Resource);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FResourceOnUnallocated, UResource*, Resource);


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


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource")
	APOTLStructure* AllocatedTo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource")
	UProviderComponent* Provider;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	float Value;

	bool Locked;

	float ExpirationTime;

	float LuxucyRating; // 1-10 factor. The richer the more luxury resource people will require.

	UStorageComponent* StoredIn;


	APOTLStructure* GetOwner();

	void Consume(EConsumeType ConsumeType = EConsumeType::Undefined, bool bRemoveFromStorage = true);

	void RemoveFromStorage();

	bool Transfer(UStorageComponent* Storage);

	bool AllocateTo(APOTLStructure* Structure);

	bool Unallocate();

	void Init();

	UPROPERTY(BlueprintAssignable, Category = "Resource|Event")
	FResourceOnAllocated OnResourceAllocated;

	UPROPERTY(BlueprintAssignable, Category = "Resource|Event")
	FResourceOnUnallocated OnResourceUnallocated;




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
