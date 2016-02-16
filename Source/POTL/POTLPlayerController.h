// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "POTLDataHolder.h"
#include "POTLGameInstance.h"
#include "POTLStructure.h"
#include "Kismet/GameplayStatics.h"
#include "POTLHUD.h"
#include "POTLPlayerController.generated.h"



//~~~~~ ENUMS ~~~~~//




//~~~~~ Delegates/Event dispatcher ~~~~~//
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHexOver, FST_Hex, Hex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHexSelected, FST_Hex, Hex);






//~~~~~ CLASS ~~~~~//
UCLASS()
class POTL_API APOTLPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	
public:

	// Sets default values for this actor's properties
	APOTLPlayerController(const FObjectInitializer &ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	UPOTLGameInstance* GameInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool")
	EToolType ActiveToolType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool")
	bool BuildingAllowed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	APOTLStructure* BuilderStructure;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	FST_Structure BuildStructureData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	int32 BaseRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn")
	FST_Hex CachedHex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	TArray<FST_Hex> CityConstructionLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	TArray<FST_Hex> BuildStructureHexes;

	//~~ Input states ~~//

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool LeftMouseButtonDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool RightMouseButtonDown;



	/*********** Delegates **************/

	UPROPERTY(BlueprintAssignable, Category = "Turn")
	FHexOver OnHexOver;

	UPROPERTY(BlueprintAssignable, Category = "Turn")
	FHexSelected OnHexSelected;


	/*********** Functions **************/

	//UFUNCTION(BlueprintCallable, Category = "Data")
	//void Draw();


	//~~ BeginPlay ~~//
	virtual void BeginPlay() override;

	//~~ Tock ~~//
	virtual void Tick(float DeltaSeconds) override;

protected:

	UFUNCTION(BlueprintCallable, Category = "Power")
	void LeftClickPressed();

	UFUNCTION(BlueprintCallable, Category = "Power")
	void LeftClickReleased();

	UFUNCTION(BlueprintCallable, Category = "Power")
	void RightClickPressed();

	UFUNCTION(BlueprintCallable, Category = "Power")
	void RightClickReleased();


protected:

	//~~ Input setup ~~//
	virtual void SetupInputComponent() override;
	//virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	

};
