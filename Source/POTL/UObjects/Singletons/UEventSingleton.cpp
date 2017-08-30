// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "UEventSingleton.h"




UEventSingleton::UEventSingleton()
{

}


UEventSingleton::~UEventSingleton()
{

}

FOnCancelRetDelegate* UEventSingleton::BindToCancelEvent(UObject* Listener, int Priority)
{
	if (Listener)
	{
		UnbindToCancelEvent(Listener);
		FOnCancelRetDelegate* Delegate = new FOnCancelRetDelegate();
		CancelDelegates.Add(Listener, Delegate);
		return Delegate;
	}
	return nullptr;
}

void UEventSingleton::UnbindToCancelEvent(UObject* Listener)
{
	if (Listener && CancelDelegates.Contains(Listener))
	{
		FOnCancelRetDelegate* Delegate = CancelDelegates[Listener];
		Delegate->Unbind();
		CancelDelegates.Remove(Listener);
		delete Delegate;
	}
}

void UEventSingleton::TriggerConfirmEvent()
{

}

void UEventSingleton::TriggerCancelEvent()
{
	for (auto& Delegate : CancelDelegates)
	{
		if (Delegate.Key && Delegate.Value && Delegate.Value->IsBound())
		{
			if (Delegate.Value->Execute())
			{
				break;
			}
		}
	}
}