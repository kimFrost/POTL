// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/UStructureComponent.h"
#include "POTLStructure.h"
#include "Components/UStructureComponent.h"
#include "UStructureTaskComponent.generated.h"


//~~~~~ Delegates/Event dispatcher ~~~~~//
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnProgressComplete);


//~~ Forward declarations ~~//
//class APOTLStructure;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POTL_API UStructureTaskComponent : public UStructureComponent
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this component's properties
	UStructureTaskComponent();



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Module")
	bool bIsWorking; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Module")
	float Progress;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Module")
	float TaskLength;


	UFUNCTION(BlueprintCallable, Category = "Time")
	virtual void OnTimeUpdate(float Time, float TimeProgressed) override;

	UFUNCTION(BlueprintCallable, Category = "Module")
	virtual void Init() override;

	UFUNCTION(BlueprintCallable, Category = "Time")
	virtual void OnProgressComplete();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

		
	
};
