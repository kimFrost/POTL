// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "POTLDataHolder.generated.h"


//~~~~~ Forward Declarations ~~~~~//
class APOTLStructure;
class UFactoryComponent;



//~~~~~ ENUMS ~~~~~//
UENUM(BlueprintType)
enum class EToolType : uint8
{
	None UMETA(DisplayName = "None"),
	Select UMETA(DisplayName = "Select"),
	PlantStructure UMETA(DisplayName = "PlantStructure"),
	ToogleAllocateHex UMETA(DisplayName = "ToogleAllocateHex"),
	DevTest UMETA(DisplayName = "DevTest")
};

UENUM(BlueprintType)
enum class EPersonGender : uint8
{
	Male UMETA(DisplayName = "Male"),
	Female UMETA(DisplayName = "Female")
};

UENUM(BlueprintType)
enum class EPersonTags : uint8
{
	Male UMETA(DisplayName = "Royal")
};

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
enum class EPersonBaseTaskList : uint8
{
	Auto UMETA(DisplayName = "Auto"),
	Build UMETA(DisplayName = "Build"),
	GatherFood UMETA(DisplayName = "Gather Food"),
	GatherWood UMETA(DisplayName = "Gather Wood"),
	GatherStone UMETA(DisplayName = "Gather Stone")
};

UENUM(BlueprintType)
enum class EAllocationType : uint8
{
	All UMETA(DisplayName = "All"),
	None UMETA(DisplayName = "None"),
	Sequence UMETA(DisplayName = "Sequence"),
	RequestDirect UMETA(DisplayName = "RequestDirect"),
	FactoryProduction UMETA(DisplayName = "FactoryProduction"),
	FactoryBilling UMETA(DisplayName = "FactoryBilling"),
	FactoryLeftover UMETA(DisplayName = "FactoryLeftover"),
	Gathered UMETA(DisplayName = "Gathered"),
	Decay UMETA(DisplayName = "Decay"),
	ProductionDecay UMETA(DisplayName = "ProductionDecay"),
	WorkForce UMETA(DisplayName = "WorkForce")
};

UENUM(BlueprintType)
enum class EResourceList : uint8
{
	Free UMETA(DisplayName = "Free"),
	Upkeep UMETA(DisplayName = "Upkeep"),
	Allocations UMETA(DisplayName = "Allocations")
};

UENUM(BlueprintType)
enum class EHighlightType : uint8
{
	Red UMETA(DisplayName = "Red"),
	Green UMETA(DisplayName = "Green"),
	Blue UMETA(DisplayName = "Blue")
};

UENUM(BlueprintType)
enum class EDecalType : uint8
{
	ValidBuild UMETA(DisplayName = "ValidBuild"),
	InvalidBuild UMETA(DisplayName = "InvalidBuild"),
	AttachPoint UMETA(DisplayName = "AttachPoint")
};


//~~~~~ Custom types ~~~~~//

struct CubeCoord
{
public:
	CubeCoord()
		: X(0)
		, Y(0)
		, Z(0)
	{}
	int X;
	int Y;
	int Z;
};

USTRUCT(BlueprintType)
struct FIdAmount
{
	GENERATED_USTRUCT_BODY()
public:
	FIdAmount()
		: Id(TEXT(""))
		, Amount(0)
	{}
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CustomType")
	FString Id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CustomType")
	int Amount;
};

 
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
	UPROPERTY(EditAnywhere, Category = "Resources")
	int32 Sequence;
};




//~~~~~ STRUCTS ~~~~~//

/*** FST_Point ***/
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


/*** FST_ConstructLocation ***/
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
	bool OnMapEdge;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
	TArray<int32> EmitDistances;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
	TArray<APOTLStructure*> AttachTo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
	TArray<APOTLStructure*> AdjacentStructures;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
	TArray<APOTLStructure*> AdjacentRootStructures;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
	TArray<APOTLStructure*> EmitTo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
	bool Blocked;

	FST_ConstructLocation()
	{
		Location = { -1, -1, -1 };
		Cube = { 0, 0, 0 };
		TreeId = TEXT("");
		OnRidge = false;
		OnMapEdge = false;
		Blocked = false;
	}
};


/*** FST_HexResources ***/
USTRUCT(BlueprintType)
struct FST_HexResourceInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	APOTLStructure* ProvideTo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	bool HasTrees;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	bool HasRiver;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	bool HasLake;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	bool HasShore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	int32 ForestDepth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	int32 LakeDepth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	TEnumAsByte<EPhysicalSurface> SurfaceType;
	//EPhysicalSurface SurfaceType;

	FST_HexResourceInfo()
	{
		ProvideTo = nullptr;
		HasTrees = false;
		HasRiver = false;
		HasLake = false;
		HasShore = false;
		ForestDepth = 0;
		LakeDepth = 0;
		SurfaceType = EPhysicalSurface::SurfaceType_Default;
	}
};


/*** FST_Hex ***/
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
	FST_HexResourceInfo Resources;

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


/*** FST_Factory ***/
USTRUCT(BlueprintType)
struct FST_Factory : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	FString Recipe;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	int32 MaxQuantity;

	// Constructor
	FST_Factory()
	{
		Recipe = "";
		MaxQuantity = -1;
	}
};


/*** FST_Factory ***/
USTRUCT(BlueprintType)
struct FST_Gatherer : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	FString Type;

	// Constructor
	FST_Gatherer()
	{
		Type = "";
	}
};

/*** FST_ConstructionStage ***/



/*** FST_Structure ***/
USTRUCT(BlueprintType)
struct FST_Structure : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	FST_Structure(
		FString Id = "",
		FString Title = "",
		FString Description = "",
		bool Burnable = false,
		TArray<FVector> CubeSizes = TArray<FVector>(),
		FVector Entrance = { 0, 0, 0 },
		int RotationDirection = 0,
		bool BlockConstruction = true,
		bool BlockPathing = true,
		float ConstructionTime = 10.f,
		TArray<FIdAmount> ConstructionCost = TArray<FIdAmount>())
		: Id(Id)
		, Title(Title)
		, Description(Description)
		, Burnable(Burnable)
		, CubeSizes(CubeSizes)
		, Entrance(Entrance)
		, RotationDirection(RotationDirection)
		, BlockConstruction(BlockConstruction)
		, BlockPathing(BlockPathing)
		, ConstructionTime(ConstructionTime)
		, ConstructionCost(ConstructionCost)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	FString Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	FString Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	bool Burnable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	TSubclassOf<APOTLStructure> StructureClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	TArray<FString> AttachTo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	TArray<FVector> CubeSizes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	FVector Entrance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	int RotationDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	bool BlockConstruction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	bool BlockPathing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	float ConstructionTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	TArray<FIdAmount> ConstructionCost;

	/*
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	TArray<FString> EmitTo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	TArray<FString> Providers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	TArray<FST_Factory> Factories;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	TArray<FST_Gatherer> Gatherers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	int32 BaseBroadcastRange;
	*/
};


/*** FST_StructureBlock ***/
USTRUCT(BlueprintType)
struct FST_StructureBlock : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	FString Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	FString Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	TArray<FString> FitsOn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	UStaticMesh* Mesh;

	FST_StructureBlock()
	{
		Id = TEXT("");
		Title = "";
		Mesh = nullptr;
	}
};


/*** FST_Skill ***/
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


/*** FST_Person ***/
USTRUCT(BlueprintType)
struct FST_Person
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Person")
	FString FirstName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Person")
	FString FamilyName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Person")
	FString NickName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Person")
	TArray<FString> Groups;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	TArray<EPersonTags> Tags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Person")
	int32 Age;

	UPROPERTY(EditAnywhere, Category = "Person")
	TMap<FString, int32> Modifiers; // Starvation, Hunger, Sick, Poisoned, Thristy, Cold, Lonely, Educated, Homeless, Power hungry, etc.

	UPROPERTY(EditAnywhere, Category = "Person")
	TMap<FString, int32> Traits; // Intelligent, Insane, Magic gifted, 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Person")
	EPersonGender Gender; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Person")
	EPersonTypesEnum Type; // Boy, Girl, Man, Women, Old man, Old women

	// Origin ? // Country / region ?

	// Father 

	// Mother

	// Children

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Person")
	APOTLStructure* Home;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Person")
	APOTLStructure* AssignedTo;

	EPersonBaseTaskList AssignedTask;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Person")
	int32 OwnIndex;

	FST_Person()
	{
		FirstName = "";
		FamilyName = "";
		NickName = "";
		Age = 0;
		Type = EPersonTypesEnum::Man;
		Home = nullptr;
		AssignedTo = nullptr;
		OwnIndex = -1;
		AssignedTask = EPersonBaseTaskList::Auto;
	}
};


/*** FST_Resource ***/
USTRUCT(BlueprintType)
struct FST_Resource : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	FString Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	FString Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	TArray<FString> Tags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	float UnitMass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	float Value;

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

	FST_Resource()
	{
		Id = TEXT("");
		Title = TEXT("");
		UnitMass = 1.f;
		Value = 0.f;
		Stackable = true;
		Burnable = true;
		StackSize = 64;
		Quantity = 0;
		MaxAge = -1;
		SortSequence = 9999;
	}
};


/*** FST_Consumable ***/
USTRUCT(BlueprintType)
struct FST_Consumable : public FST_Resource
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	FString Something;

	FST_Consumable()
	{
		Something = TEXT("");
	}
};


/*** FST_ResourceRequest ***/
USTRUCT(BlueprintType)
struct FST_ResourceRequest
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	APOTLStructure* From;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	UFactoryComponent* Factory;

	UPROPERTY(EditAnywhere, Category = "Resource")
	TMap<FString, int32> Request;

	UPROPERTY(EditAnywhere, Category = "Resource")
	TMap<FString, int32> Payoff;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	int32 TestRequestQuantity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	int32 TestPayoffQuantity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	int32 Sequence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	int32 Steps;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	EAllocationType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	bool Consume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	bool Bubble;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	bool RequestMet;

	// Allocation ref index


	FST_ResourceRequest()
	{
		From = nullptr;
		Factory = nullptr;
		Sequence = 0;
		Steps = 0;
		Type = EAllocationType::None;
		Consume = false;
		Bubble = true;
		RequestMet = false;
		TestRequestQuantity = 0;
		TestPayoffQuantity = 0;
	}
};


/*** FST_ResourceAllocation ***/
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	int32 Sequence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	int32 PersonIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	bool Locked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	int32 LockedMin;

	FST_ResourceAllocation()
	{
		From = nullptr;
		To = nullptr;
		ResourceKey = "";
		Quantity = 0;
		Type = EAllocationType::None;
		Sequence = 0;
		PersonIndex = -1;
		Locked = false;
		LockedMin = 0;
	}
};


/*** FST_ResourceAllocation ***/
USTRUCT(BlueprintType)
struct FST_ResourceAlteration
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	FString Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	int32 Storage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	int32 Alteration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	int32 Decay;

	FST_ResourceAlteration()
	{
		Id = "";
		Storage = 0;
		Alteration = 0;
		Decay = 0;
	}
};


/*** FST_TMap ***/
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





UCLASS()
class POTL_API UPOTLDataHolder : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UPOTLDataHolder(const FObjectInitializer& ObjectInitializer);

};


/*
class POTL_API POTLDataHolder
{
public:
POTLDataHolder();
~POTLDataHolder();
};
*/