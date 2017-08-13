// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "POTLDataHolder.h"
#include "AStructureBuilder.generated.h"

UCLASS()
class POTL_API AStructureBuilder : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStructureBuilder();

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Builder")
	//FST_Structure StructureBaseData;

	//void SetOnTile();
	//void SetLocation();
	//void Build();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
