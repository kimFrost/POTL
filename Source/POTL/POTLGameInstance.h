// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "POTLUtilFunctionLibrary.h"
#include "POTLGameInstance.generated.h"


//~~~~~ Forward Declarations ~~~~~//
class APOTLStructure;


//~~~~~ ENUMS ~~~~~//
UENUM(BlueprintType)
enum class EPersonTypesEnum : uint8
{
	Boy UMETA(DisplayName = "Boy"),
	Girl UMETA(DisplayName = "Girl"),
	Man UMETA(DisplayName = "Man"),
	Women UMETA(DisplayName = "Women"),
	OldMan UMETA(DisplayName = "OldMan"),
	OldWomen UMETA(DisplayName = "OldWomen")
};

UENUM(BlueprintType)
enum class EAllocationType : uint8
{
	All UMETA(DisplayName = "All"),
	None UMETA(DisplayName = "None"),
	RequestDirect UMETA(DisplayName = "RequestDirect"),
	FactoryProduction UMETA(DisplayName = "FactoryProduction"),
	FactoryBilling UMETA(DisplayName = "FactoryBilling"),
	FactoryLeftover UMETA(DisplayName = "FactoryLeftover")
};

UENUM(BlueprintType)
enum class EResourceList : uint8
{
	Free UMETA(DisplayName = "Free"),
	Upkeep UMETA(DisplayName = "Upkeep")
};


//~~~~~ Delegates/Event dispatcher ~~~~~//
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTurnSwitched);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams();
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTurnSwitched, float, Turn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNewTurn, float, Turn);
//DECLARE_EVENT(FTurnSwitched);



 
//~~~~~ TEST DATA IMPORT ~~~~~//
USTRUCT(BlueprintType)
struct FItemInformation : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	FItemInformation() 
		: Id(FName(TEXT("")))
		, ItemName(TEXT(""))
		, ItemCost(0.f)
		, ItemInfo(TEXT(""))
	{}
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Information System")
	FName Id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Information System")
	FString ItemName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Information System")
	float ItemCost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Information System")
	FString ItemInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Information System")
	TArray<FName> ItemList;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkillData)
	//TAssetPtr<UTexture> SkillIcon;
};


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
struct FST_Ingredient
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	FName Id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	int32 Quantity;
	FST_Ingredient()
		: Id(FName(TEXT("")))
		, Quantity(0)
	{}
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
	TArray<FST_Ingredient> Ingredients;
	UPROPERTY(EditAnywhere, Category = "Resources")
	int32 Servings;
};




//~~~~~ STRUCTS ~~~~~//
USTRUCT(BlueprintType)
struct FST_Point
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Point")
	FVector Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Point")
	int32 Column;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Point")
	int32 Row;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Point")
	bool IsCreator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Point")
	bool Exits;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Point")
	int32 Hex0Index;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Point")
	int32 Hex1Index;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Point")
	int32 Hex2Index;

	FST_Point()
	{
		Location = { -1, -1, -1 };
		Column = 0;
		Row = 0;
		IsCreator = false;
		Exits = false;
		Hex0Index = -1;
		Hex1Index = -1;
		Hex2Index = -1;
	}
};

USTRUCT(BlueprintType)
struct FST_ConstructLocation
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
	FVector Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
	FVector Cube;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
	FString TreeId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
	bool OnRidge;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
	TArray<APOTLStructure*> AttachTo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
	TArray<APOTLStructure*> EmitTo;

	FST_ConstructLocation()
	{
		Location = { -1, -1, -1 };
		Cube = { 0, 0, 0 };
		TreeId = TEXT("");
		OnRidge = false;
	}
};


USTRUCT(BlueprintType)
struct FST_Hex
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FVector Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FRotator Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FST_Point Point0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FST_Point Point1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FST_Point Point2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FST_Point Point3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FST_Point Point4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FST_Point Point5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	bool Remove;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	APOTLStructure* AttachedBuilding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	AActor* AttachedActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	AActor* AttachedMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FVector HexCubeCoords;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FVector2D HexOffsetCoords;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	int32 HexIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	TArray<int32> HexNeighborIndexes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	//TArray<FST_ConstructLocation> ConstructLocations;
	FST_ConstructLocation ConstructInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	AActor* HighlightDecal; 
	// This caused a Pointer Access violation somewhere. Thus crash the game on the first gabage collection.
	// NOTE: It was caused by not having it be initiate in FST_Hex() to be nullptr

	// [2015.09.28-17.57.23:650][ 63]LogWindows: === Critical error: ===
	//Fatal error : [File:D : \BuildFarm\buildmachine_++depot + UE4 - Releases + 4.9\Engine\Source\Runtime\CoreUObject\Private\UObject\GarbageCollection.cpp] [Line:383]
	// Invalid object in GC : 0x000007fe028f000a, ReferencingObject : GI_POTL_C / Engine / Transient.UnrealEdEngine_0 : GI_POTL_C_0, ReferencingObjectClass : BlueprintGeneratedClass / Game / Blueprint / GameInstance / GI_POTL.GI_POTL_C, Property Name : HighlightDecal, Offset : 384, TokenIndex : 16

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	//TArray<FST_ConstructLocation> ConstructLocations;
	bool DebugMe;

	FST_Hex()
	{
		Location = { -1, -1, -1 };
		Rotation = { -1, -1, -1 };
		//Point0 = nullptr;
		//Point1 = nullptr;
		//Point2 = nullptr;
		//Point3 = nullptr;
		//Point4 = nullptr;
		//Point5 = nullptr;
		Remove = false;
		AttachedBuilding = nullptr;
		AttachedActor = nullptr;
		AttachedMesh = nullptr;
		HexCubeCoords = {-1, -1, -1};
		HexOffsetCoords = {-1, -1};
		HexIndex = -1;
		HexNeighborIndexes.Init(-1, 6);
		HighlightDecal = nullptr;
		DebugMe = false;
	}
};

USTRUCT(BlueprintType)
struct FST_Structure : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	FString Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	FString Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	bool Burnable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	//class APOTLStructure* StructureClass;
	TSubclassOf<APOTLStructure> StructureClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	TArray<FString> AttachTo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	TArray<FString> EmitTo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	int32 BaseBroadcastRange;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	TArray<FVector> CubeSizes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	FVector CubeRotationRoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	FVector BroadcastRoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	int32 RotationDirection;

	FST_Structure()
	{
		Id = TEXT("");
		Title = "";
		Burnable = false;
		BaseBroadcastRange = 0;
		CubeRotationRoot = { 0, 0, 0 };
		BroadcastRoot = { 0, 0, 0 };
		RotationDirection = 0;
	}
};


USTRUCT(BlueprintType)
struct FST_Person
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Person")
	FString Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Person")
	FName FamilyName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Person")
	TArray<FName> Groups;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Person")
	int32 Age;

	UPROPERTY(EditAnywhere, Category = "Person")
	TMap<FName, int32> Modifiers; // Starvation, Hunger, Cold, Lonely, Insane, Power hungry, etc.

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Person")
	EPersonTypesEnum Type; // Boy, Girl, Man, Women, Old man, Old women


	FST_Person()
	{
		Title = "";
		FamilyName = FName(TEXT(""));
		Age = 0;
		Type = EPersonTypesEnum::Man;
	}
};


USTRUCT(BlueprintType)
struct FST_Resource : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	FString Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	FString Title;

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

	FST_Resource()
	{
		Id = TEXT("");
		Title = TEXT("");
		UnitMass = 1.f;
		Stackable = true;
		Burnable = true;
		StackSize = 64;
		Quantity = 0;
	}
};


USTRUCT(BlueprintType)
struct FST_ResourceAllocation
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	APOTLStructure* From;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	APOTLStructure* To;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	FString ResourceKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	int32 Quantity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	EAllocationType Type;

	FST_ResourceAllocation()
	{
		From = nullptr;
		To = nullptr;
		ResourceKey = "";
		Quantity = 0;
		Type = EAllocationType::None;
	}
};


USTRUCT(BlueprintType)
struct FST_Skill
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	FString Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	float XP;

	FST_Skill()
	{
		Title = TEXT("");
		XP = 0.f;
	}
};


USTRUCT(BlueprintType)
struct FST_TMap
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	FName Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	int32 Value;

	FST_TMap()
	{
		Id = FName(TEXT(""));
		Value = 0.f;
	}
};

USTRUCT(BlueprintType)
struct FST_Factory
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "Resource")
	TMap<FString, int32> Requirements;

	UPROPERTY(EditAnywhere, Category = "Resource")
	//TMap<FName, int32> Invoice;
	TMap<FString, int32> Invoice;


	//~~ Calculate Requirements for total requirements ~~//
	void const ProcessInvoice(UDataTable* RecipeTable)
	{
		Requirements.Empty();
		if (RecipeTable)
		{
			for (auto& InvoiceItem : Invoice)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, InvoiceItem.Key);
				static const FString ContextString(TEXT("GENERAL")); //~~ Key value for each column of values ~~//
				//FST_ResourceRecipe* Recipe = RecipeTable->FindRow<FST_ResourceRecipe>(InvoiceItem.Key, ContextString);
				FST_ResourceRecipe* Recipe = RecipeTable->FindRow<FST_ResourceRecipe>(*InvoiceItem.Key, ContextString);
				if (Recipe)
				{
					for (auto& Ingredient : Recipe->Ingredients)
					{
						if (Requirements.Contains(Ingredient.Id.ToString()))	Requirements[Ingredient.Id.ToString()] += Ingredient.Quantity;
						else													Requirements.Add(Ingredient.Id.ToString(), Ingredient.Quantity);
					}
				}
			}
		}
	}

	//~~ Resolve factory ~~//
	void const Resolve(APOTLStructure* Caller, TMap<FString, int32>& FreeResources, UDataTable* RecipeTable, TMap<FString, int32>& Production, TMap<FString, int32>& Billing)
	{
		if (RecipeTable)
		{ 
			//~~ The production ~~//
			int32 i = 0;
			//TArray<FName> IdList;
			//Invoice.GenerateKeyArray(IdList);
			TArray<int32> ValueList;
			Invoice.GenerateValueArray(ValueList);

			//~~ Calculate total singleton length of production ~~//
			int32 SingletonLength = 0;
			for (i = 0; i < ValueList.Num(); i++)
			{
				SingletonLength += ValueList[i];
			}

			TMap<FString, int32> InvoiceCopy = Invoice;
			TMultiMap<FString, int32> SingletonQue;
			//~~ Loop through SingletonLength and pull values from the invoice ~~//
			for (i = 0; i < SingletonLength; i++)
			{
				if (SingletonQue.Num() < SingletonLength)
				{
					for (auto& InvoiceItem : InvoiceCopy)
					{
						if (InvoiceItem.Value > 0)
						{
							SingletonQue.Add(InvoiceItem.Key, 1);
							InvoiceItem.Value -= 1;
						}
					}
				}
				else
				{
					break;
				}
			}

			//~~ Produce items ~~//
			bool InvoiceFulfilled = true;
			for (auto& Singleton : SingletonQue)
			{
				static const FString ContextString(TEXT("GENERAL")); //~~ Key value for each column of values ~~//
				//FST_ResourceRecipe* Recipe = RecipeTable->FindRow<FST_ResourceRecipe>(Singleton.Key, ContextString);
				FST_ResourceRecipe* Recipe = RecipeTable->FindRow<FST_ResourceRecipe>(*Singleton.Key, ContextString);
				if (Recipe) //~~ If recipe for invoce item is found ~~//
				{
					bool ResourcesRequirementFulfilled = true;
					for (auto& Ingredient : Recipe->Ingredients)
					{
						if (Ingredient.Id.ToString() != "")
						{
							if (FreeResources.Contains(Ingredient.Id.ToString()))
							{
								GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Magenta, "Ingredient.Id.ToString(): " + Ingredient.Id.ToString());
								GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Magenta, "FreeResources[Ingredient.Id.ToString()]: " + FString::FromInt(FreeResources[Ingredient.Id.ToString()]));
								GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Magenta, "Ingredient.Quantity: " + FString::FromInt(Ingredient.Quantity));
								if (FreeResources[Ingredient.Id.ToString()] < Ingredient.Quantity)
								{
									ResourcesRequirementFulfilled = false;
									GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Magenta, "ResourcesRequirementFulfilled FALSE");
								}
							}
							else {
								ResourcesRequirementFulfilled = false;
							}
						}
						else {
							ResourcesRequirementFulfilled = false;
						}
					}
					//~~ If there are enough resources for the single item production, then produce it and consume the resources ~~//
					if (ResourcesRequirementFulfilled)
					{
						//~~ Remove resources ~~//
						for (auto& Ingredient : Recipe->Ingredients)
						{
							if (Billing.Contains(Ingredient.Id.ToString()))		Billing[Ingredient.Id.ToString()] += Ingredient.Quantity;
							else												Billing.Add(Ingredient.Id.ToString(), Ingredient.Quantity);
						}
						//~~ Add to production ~~//
						Production.Add(Singleton.Key, Recipe->Servings);
					}
				}
			}
			GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, "Production.Num(): " + FString::FromInt(Production.Num()));
		}
	}
	// Constructor
	FST_Factory()
	{
		
	}
};



/**
*
*/
UCLASS()
class POTL_API UPOTLGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	// Sets default values for this's properties
	UPOTLGameInstance(const FObjectInitializer &ObjectInitializer);


	/*********** VARIABLES **************/

	// Map
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Map")
	float HexWidth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Calculated")
	float HexHeight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Calculated")
	int32 GridXCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Calculated")
	int32 GridYCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Map")
	TArray<FST_Point> Points;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Map")
	TArray<FST_Hex> Hexes;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Map")
	AActor* Landscape;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Resources")
	//UDataTable* RecipeTable;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Structure")
	//UDataTable* StructureTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Structure")
	UDataTable* DATA_Recipes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Structure")
	UDataTable* DATA_Structures;

	FTimerHandle TurnTimerHandle;

	//~~ Structure ~~//

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Structure")
	TArray<APOTLStructure*> Structures;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Structure")
	TArray<APOTLStructure*> RootStructures;

	//~~ Debug ~~//
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Debug")
	FString FindmeString;

	/*********** FUNCTIONS **************/

	//~~ Data ~~//

	UFUNCTION(BlueprintCallable, Category = "Data")
	void ReadTables();

	//~~ Structure ~~//

	UFUNCTION(BlueprintCallable, Category = "Structure")
	TArray<FST_Hex> GetConstructLocations(APOTLStructure* Structure, bool IncludeChildren);

	UFUNCTION(BlueprintCallable, Category = "Structure")
	TArray<int32> GetConstructLocationIndexes(APOTLStructure* Structure, bool IncludeChildren);

	UFUNCTION(BlueprintCallable, Category = "Structure")
	bool IsHexBuildable(FST_Hex& Hex);

	UFUNCTION(BlueprintCallable, Category = "Structure")
	APOTLStructure* PlantStructure(FVector CubeCoord, FString RowName, FString TreeId, APOTLStructure* EmitTo, bool InstaBuild);

	UFUNCTION(BlueprintCallable, Category = "Structure")
	void CreateStructureConnection(APOTLStructure* From, APOTLStructure* To);

	//~~ Map - Creation ~~//

	UFUNCTION(BlueprintCallable, Category = "Map")
	void TraceLandscape(ECollisionChannel CollisionChannel);

	UFUNCTION(BlueprintCallable, Category = "Map")
	void CreateHexes(ECollisionChannel CollisionChannel);

	UFUNCTION(BlueprintCallable, Category = "Map")
	void CleanHexes();

	UFUNCTION(BlueprintCallable, Category = "Map")
	void EnrichHexes();

	UFUNCTION(BlueprintCallable, Category = "Map")
	void CalcHexesRot();

	//~~ Turn ~~//

	UFUNCTION(BlueprintCallable, Category = "Turn")
	void SwitchTurn();

	UFUNCTION(BlueprintCallable, Category = "Turn")
	void NewTurn(float WaitTime);

	//~~ Util - Hex ~~//

	UFUNCTION(BlueprintCallable, Category = "Util")
	FST_Hex MouseToHex(ECollisionChannel CollisionChannel);

	UFUNCTION(BlueprintCallable, Category = "Util")
	FST_Hex LocationToHex(FVector Location);

	//~~ Debug - Log ~~//

	UFUNCTION(BlueprintCallable, Category = "Debug")
	static void Log(FString Msg, float Duration, FColor DebugColor, int32 GroupIndex);


	/*********** Delegates **************/

	UPROPERTY(BlueprintAssignable, Category = "Turn")
	FTurnSwitched OnTurnSwitched;

	UPROPERTY(BlueprintAssignable, Category = "Turn")
	FNewTurn OnNewTurn;
};
