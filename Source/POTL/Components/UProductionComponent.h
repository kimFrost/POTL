// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/UStructureComponent.h"
#include "UProductionComponent.generated.h"



//~~ DELEGATES ~~//
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnProduction, float, Amount);



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class POTL_API UProductionComponent : public UStructureComponent
{
	GENERATED_BODY()

public:
	
	// Sets default values for this component's properties
	UProductionComponent();

	// Production Item and quantity
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Module")
	TMap<FString, int> Production;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Production")
	void OnProduction();

	virtual void OnProgressComplete() override;

};
