// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "POTLStructure.h"
#include "UStructureComponent.generated.h"


//~~~~~ Delegates/Event dispatcher ~~~~~//


//~~ Forward declarations ~~//


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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Module")
	bool bIsWorking; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Module")
	bool bIsOn; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Module")
	bool bRequireAttached;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Module")
	//float Progress;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Module")
	//float TaskLength;

	UFUNCTION(BlueprintCallable, Category = "Module")
	bool ToggleOn();


	UFUNCTION(BlueprintCallable, Category = "Time")
	virtual void OnTimeUpdate(float Time, float TimeProgressed);

	UFUNCTION(BlueprintNativeEvent, Category = "Module")
	void OnInit();

	UFUNCTION(BlueprintCallable, Category = "Module")
	virtual void Init();

	//UFUNCTION(BlueprintCallable, Category = "Time")
	//virtual void OnProgressComplete();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;
		
	
};
