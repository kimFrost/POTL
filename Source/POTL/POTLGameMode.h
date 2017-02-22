// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "POTLGameMode.generated.h"


//~~~~~ Delegates/Event dispatcher ~~~~~//
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTimeUpdated, float, Time, float, TimeProgressed);



UCLASS()
class POTL_API APOTLGameMode : public AGameMode
{
	GENERATED_BODY()
	
	// Sets default values for this actor's properties
	APOTLGameMode(const FObjectInitializer &ObjectInitializer);

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	float Time;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	float PlayRate;

	UPROPERTY(BlueprintAssignable, Category = "Time")
	FOnTimeUpdated OnTimeUpdated;
	

	UFUNCTION(BlueprintCallable, Category = Turn)
	void ProgressTime(float Amount);


	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;


};
