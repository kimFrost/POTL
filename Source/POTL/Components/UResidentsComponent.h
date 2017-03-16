// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UStructureComponent.h"
#include "POTLStructure.h"
#include "UResidentsComponent.generated.h"


//~~~~~ Delegates/Event dispatcher ~~~~~//


//~~ Forward declarations ~~//


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POTL_API UResidentsComponent : public UStructureComponent
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this component's properties
	UResidentsComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resident")
	float ResidentDensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resident")
	int NumOfHoseholds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resident")
	float Wealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resident")
	float Food;

	bool bIsReligious;

	float ConsumptionRate;

	FTimerHandle NeedsCheckTimer;

	void CheckNeeds();


	virtual void Init() override;

	virtual void OnTimeUpdate(float Time, float TimeProgressed) override;

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;
	
};
