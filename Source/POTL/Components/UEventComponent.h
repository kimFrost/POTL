// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "UEventComponent.generated.h"


//~~~~~ Delegates/Event dispatcher ~~~~~//
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCancelDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnConfirmDelegate);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCancelDelegate, const TArray<FST_ResourceQuantity>&, TotalProduction);


//~~ Forward declarations ~~//


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POTL_API UEventComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this component's properties
	UEventComponent();


public:
	void InitializeComponent() override;
	
	



	// DELEGATES

	UPROPERTY(BlueprintAssignable, Category = "Global|Event")
	FOnCancelDelegate OnCancelEvent;

	UPROPERTY(BlueprintAssignable, Category = "Global|Event")
	FOnConfirmDelegate OnConfirmEvent;

	// NATIVE EVENTS


};
