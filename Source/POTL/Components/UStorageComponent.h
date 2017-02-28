// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/UStructureComponent.h"
#include "UObjects/UResource.h"
#include "UStorageComponent.generated.h"



//~~ DELEGATES ~~//
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStorageUpdate);



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class POTL_API UStorageComponent : public UStructureComponent
{
	GENERATED_BODY()

public:
	
	// Sets default values for this component's properties
	UStorageComponent();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Storage")
	int StorageCapacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Storage")
	TArray<FString> AllowedResources;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Storage")
	TMap<FString, int> StoredResources;
	//TArray<UResource*> StoredResources;

	// ?? Locked for internal use ??
	// ?? Storage for production ??

	UFUNCTION(BlueprintCallable, Category = "Storage")
	int AddResource(FString ResourceId, int Quantity);
	
	UFUNCTION(BlueprintCallable, Category = "Storage")
	void StoreResource(UResource* Resource);

	bool RequestResouce(APOTLStructure* Requester, FString ResourceId, int Quantity);

	UPROPERTY(BlueprintAssignable, Category = "Storage|Event")
	FOnStorageUpdate OnStorageUpdate;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Storage")
	void StorageUpdate();



	// Called when the game starts
	virtual void BeginPlay() override;


	//~~ Options variables ~~//

	/*
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Module")
	float StorageCapacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Module")
	float PowerUpkeep;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Module")
	float PowerGenerated;

	//~~ Runtime variables ~~//

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Module")
	float StoredPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Module")
	float SurplusPower;
	

	UFUNCTION(BlueprintCallable, Category = "Ship")
	float GeneratePower();





	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	*/

};
