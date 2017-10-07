// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "UObjects/UResource.h"
#include "POTLDataHolder.h"
#include "POTLGameInstance.h"
#include "UObjects/UHexTile.h"
#include "POTLStructure.generated.h"


//~~~~~ Forward Declarations ~~~~~//
class UConstructionComponent;
class UEventComponent;
class UGatherComponent;
class UProviderComponent;
class ARangeDecal;


//~~ DELEGATES ~~//
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllocatedHexesChanged);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStructureClicked, AActor*, TouchedActor, FKey, ButtonPressed);
//DECLARE_DELEGATE_RetVal_OneParam(EHandleType, FOnStructureClickedDelegate, bool);
//DECLARE_DELEGATE_RetVal(EHandleType, FOnStructureClickedDelegate);
DECLARE_DELEGATE_RetVal_OneParam(EHandleType, FOnStructureClickedDelegate, APOTLStructure*);
DECLARE_DELEGATE_RetVal_OneParam(EHandleType, FOnHexAllocateDelegate, UHexTile*);
DECLARE_DELEGATE_RetVal_OneParam(EHandleType, FOnHexUnallocateDelegate, UHexTile*);


//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnConstructionComplete);


//~~~~~ STRUCTS ~~~~~//
 
//~~~~~ ENUMS ~~~~~//




UCLASS()
class POTL_API APOTLStructure : public AActor
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this actor's properties
	APOTLStructure(const FObjectInitializer &ObjectInitializer);
	//AVehicle(const class FPostConstructInitializeProperties& PCIP, FString Path, FString Name);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	UPOTLGameInstance* GameInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	APOTLStructure* AttachedTo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	TArray<APOTLStructure*> AttachedStructures;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure")
	TArray<UGatherComponent*> GatherComponents;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure")
	TArray<UProviderComponent*> ProviderComponents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	UHexTile* BaseHex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	TArray<UHexTile*> HexesInRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	TArray<APOTLStructure*> RootStructuresInRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	TArray<APOTLStructure*> StructuresInRange;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure")
	TArray<UHexTile*> AllocatedHexes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	TArray<UHexTile*> OccupiedHexes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	int32 HexIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	FVector CubeCoord;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	FVector CubeCoords;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	FST_Structure StructureBaseData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	FString StructureRowName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure")
	ARangeDecal* RangeDecal;

	/** Construction */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	bool IsPlaceholder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	bool IsUnderConstruction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	bool bIsInitialized;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	bool BlockPathing;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure")
	UConstructionComponent* ConstructionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure")
	UEventComponent* EventComponent;


	bool bSelected;
	bool bInEditMode;

	//FTimerHandle ConstructionProgressCheckTimer;


	//OnConstructionProgress(float ProcentConstructed)


	/*********** FUNCTIONS **************/

	/** Util */

	bool Select();
	bool Deselect();

	/*
	// bool CancelSelect() const
	FORCEINLINE bool CancelSelect()
	{
		
		return false;
		//return Func; Native
	}
	*/

	UFUNCTION(BlueprintCallable, Category = "Structure")
	void EnterEditMode();

	bool LeaveEditMode();

	void UpdateInRangeLists(bool bUpdateOthers);

	void DrawInRangeInfo();


	TMap<UObject*, FOnHexAllocateDelegate*> OnHexAllocateDelegates;
	FOnHexAllocateDelegate* BindToOnHexAllocate(UObject* Listener, int Priority);
	void UnbindToOnHexAllocate(UObject* Listener);

	TMap<UObject*, FOnHexUnallocateDelegate*> OnHexUnallocateDelegates;
	FOnHexUnallocateDelegate* BindToOnHexUnallocate(UObject* Listener, int Priority);
	void UnbindToOnHexUnallocate(UObject* Listener);


	UFUNCTION(Category = "Structure")
	EHandleType ToggleAllocateHex(UHexTile* Hex, bool bUpdate);

	bool AllocateHex(UHexTile* Hex);
	bool UnallocateHex(UHexTile* Hex);

	/** Resources */

	// NEW RESOURCE SYSTEM

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure")
	TArray<UResource*> AllocatedResources;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure")
	TMap<APOTLStructure*, TArray<UResource*>> AllocatedResourcesByStructure;

	UFUNCTION(BlueprintCallable, Category = "Resources")
	void AllocateResource(UResource* Resource);

	UFUNCTION(BlueprintCallable, Category = "Resources")
	void UnallocateResource(UResource* Resource);

	void CheckOperationRequirements();

	// OLD RESOURCE SYSTEM 

	UFUNCTION(BlueprintCallable, Category = "Resources")
	int AddResource(FString ResourceId, int32 Quantity); // , EResourceList Type

	UFUNCTION(BlueprintCallable, Category = "Resources")
	bool StoreResource(UResource* Resource);

	void AddWealth(float Amount);

	void SubtractWealth(float Amount);

	// Security. Constant. Church gives security if religious

	//bool AddNeed(ENeedType Need, float Amount);


	/** Construction */

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Construction")
	void UpdateConstrunction();
	
	UFUNCTION(BlueprintCallable, Category = "Construction")
	void Init();

	UFUNCTION(BlueprintCallable, Category = "Construction")
	void RemoveStructure();

	UFUNCTION(BlueprintCallable, Category = "Construction")
	void ProcessBaseData();

	UFUNCTION(BlueprintCallable, Category = "Construction")
	void AttachToStructure(APOTLStructure* Structure);

	UFUNCTION(BlueprintCallable, Category = "Construction")
	void DetachFromStructure();

	UFUNCTION(BlueprintCallable, Category = "Construction")
	void CompleteConstruction();


	UResource* RequestResource(FString ResourceId, bool bBubble);
	UResource* RequestResourceByTag(FString TagId, bool bBubble);

	bool RequestLabor(int Amount);
	bool StoreLabor(int Amount);


	/** Map */

	UFUNCTION(BlueprintCallable, Category = "Map")
	APOTLStructure* GetNearestStructure();


	FOnAllocatedHexesChanged OnAllocatedHexesChangedDelegate;


	//TArray<FOnStructureClickedDelegate> OnStrucureClickedDelegates;
	TMap<UObject*, FOnStructureClickedDelegate*> OnStrucureClickedDelegates;

	FOnStructureClickedDelegate* BindToOnStructureClicked(UObject* Listener, int Priority);
	void UnbindToStructureClicked(UObject* Listener);
	void ClickStructure();

	UFUNCTION(BlueprintNativeEvent, Category = "Structure")
	void OnStructureClicked();


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Time")
	void OnTimeUpdate(float Time, float TimeProgressed);

	UFUNCTION(BlueprintNativeEvent, Category = "Structure")
	void OnInit();

	UFUNCTION(BlueprintNativeEvent, Category = "Structure")
	void OnConstructionComplete();

	UFUNCTION(BlueprintNativeEvent, Category = "Structure")
	void OnRemoveStructure();

	UFUNCTION(BlueprintNativeEvent, Category = "Structure")
	void OnShowResourceInformation();

	UFUNCTION(BlueprintNativeEvent, Category = "Structure")
	void OnSelected();

	UFUNCTION(BlueprintNativeEvent, Category = "Structure")
	void OnDeselected();

	UFUNCTION(BlueprintNativeEvent, Category = "Structure")
	void OnEnterEditMode();

	UFUNCTION(BlueprintNativeEvent, Category = "Structure")
	void OnLeaveEditMode();

	UFUNCTION(BlueprintNativeEvent, Category = "Structure")
	void OnAllocatedHexesChanged();

	UFUNCTION(BlueprintNativeEvent, Category = "Structure")
	void OnHexAllocated(UHexTile* Hex);

	UFUNCTION(BlueprintNativeEvent, Category = "Structure")
	void OnHexUnallocated(UHexTile* Hex);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
};

