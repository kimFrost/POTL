// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "POTLGameInstance.h"
#include "POTLStructure.generated.h"

UCLASS()
class POTL_API APOTLStructure : public AActor
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this actor's properties
	APOTLStructure(const FObjectInitializer &ObjectInitializer);
	//AVehicle(const class FPostConstructInitializeProperties& PCIP, FString Path, FString Name);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	TArray<APOTLStructure*> BroadcastTo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	TArray<APOTLStructure*> EmitTo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	bool IsRoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	FName TreeId;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	//FST_Hex Hex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	int32 HexIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	FST_Struture StructureBaseData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	int32 BroadcastRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	TArray<int32> BroadcastGridHexIndexes;



	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
};
