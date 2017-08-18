// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "POTLDataHolder.h"
#include "POTLGameInstance.h"
#include "UObjects/UHexTile.h"
#include "POTLStructure.h"
#include "Actors/StructureBuilder/AStructureBuilder.h"
#include "Kismet/GameplayStatics.h"
#include "POTLHUD.h"
#include "POTLPlayerController.generated.h"



//~~~~~ ENUMS ~~~~~//




//~~~~~ Delegates/Event dispatcher ~~~~~//
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHexOver, UHexTile*, Hex);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHexSelected, UHexTile*, Hex);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStructureSelected, APOTLStructure*, Structure);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStructureDeselected, APOTLStructure*, Structure);






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
	AStructureBuilder* StructureBuilder;
	//APOTLStructure* BuilderStructure;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	FST_Structure BuildStructureData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	int32 BaseRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn")
	UHexTile* CachedHex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	TArray<UHexTile*> CityConstructionLocations;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure") //~~ Strucutre reference cannot be a uproperty. Not supported in editor ~~/
	UHexTile* BuildBroadcastRootHex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	TArray<UHexTile*> BuildStructureHexes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	TArray<UHexTile*> ValidStructurePlaceHexes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool")
	TArray<AHexDecal*> ConstructDecals;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool")
	bool BuildValid;

	TArray<APOTLStructure*> SelectedStructures;
	APOTLStructure* ActiveStructure;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool")
	FString BuildMsg;


	//~~ Input states ~~//

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool LeftMouseButtonDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool RightMouseButtonDown;




	/*********** Functions **************/

	//UFUNCTION(BlueprintCallable, Category = "Data")
	//void Draw();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void RotateStructure();

	UFUNCTION(BlueprintCallable, Category = "Tool")
	void SetToolType(EToolType ToolType);

	void SelectStructure(APOTLStructure* Structure);
	void DeselectSelectedStructures();
	void DeselectAllStructures();

	void EditStructure(APOTLStructure* Structure);

	//~~ BeginPlay ~~//
	virtual void BeginPlay() override;

	//~~ Tock ~~//
	virtual void Tick(float DeltaSeconds) override;


	/*********** Delegates **************/

	UPROPERTY(BlueprintAssignable, Category = "Tool")
	FHexOver OnHexOver;

	//UPROPERTY(BlueprintAssignable, Category = "Tool")
	//FHexSelected OnHexSelected;


protected:

	UFUNCTION(BlueprintCallable, Category = "Input")
	void LeftClickPressed();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void LeftClickReleased();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void RightClickPressed();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void RightClickReleased();


protected:

	//~~ Input setup ~~//
	virtual void SetupInputComponent() override;
	//virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	

};
