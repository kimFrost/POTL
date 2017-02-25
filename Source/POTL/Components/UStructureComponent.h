// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "UStructureComponent.generated.h"


//~~~~~ Delegates/Event dispatcher ~~~~~//
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnProgressComplete);


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

	UPROPERTY(BlueprintReadWrite, Category = "Module")
	float TaskLength;

	UFUNCTION(BlueprintCallable, Category = "Module")
	bool ToggleOn();

	UFUNCTION(BlueprintCallable, Category = "Time")
	void OnTimeUpdate(float Time, float TimeProgressed);

	//UPROPERTY(BlueprintAssignable, Category = "Time|Module")
	//FOnProgressComplete OnProgressComplete;


	UFUNCTION(BlueprintCallable, Category = "Time")
	virtual void OnProgressComplete();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

		
	
};
