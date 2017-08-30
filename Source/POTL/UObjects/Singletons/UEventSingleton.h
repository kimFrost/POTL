// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "UEventSingleton.generated.h"


//~~~~~ Forward Declarations ~~~~~//
//class UHexTile;

//~~~~~ Delegates ~~~~~//
DECLARE_DELEGATE_RetVal(bool, FOnCancelRetDelegate);
//DECLARE_DELEGATE_RetVal_OneParam(EHandleType, FOnCancelDelegate, bool);


UCLASS(Blueprintable, BlueprintType)
class POTL_API UEventSingleton : public UObject
{
	GENERATED_BODY()

public:
	UEventSingleton();
	~UEventSingleton();


	FOnCancelRetDelegate* BindToCancelEvent(UObject* Listener, int Priority);

	void UnbindToCancelEvent(UObject* Listener);

	void TriggerConfirmEvent();
	void TriggerCancelEvent();

private:

	//TArray<FOnCancelRetDelegate*> CancelDelegates;
	TMap<UObject*, FOnCancelRetDelegate*> CancelDelegates;

	

};
