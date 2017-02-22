// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "UStructureComponent.generated.h"

//~~ Forward declarations ~~//
class APOTLStructure;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POTL_API UStructureComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this component's properties
	UStructureComponent();


	UPROPERTY(BlueprintReadWrite, Category = "Module")
	APOTLStructure* ParentStructure;

	UPROPERTY(BlueprintReadWrite, Category = "Module")
	bool bCanToggle;

	UPROPERTY(BlueprintReadWrite, Category = "Module")
	bool bIsOn;

	UPROPERTY(BlueprintReadWrite, Category = "Module")
	float Progress;

	UFUNCTION(BlueprintCallable, Category = "Order")
	bool ToggleOn();


	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

		
	
};
