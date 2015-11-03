/*
This file is part of Coherent UI, modern user interface library for
games.

Copyright (c) 2012-2014 Coherent Labs AD and/or its licensors. All
rights reserved in all media.

The coded instructions, statements, computer programs, and/or related
material (collectively the "Data") in these files contain confidential
and unpublished information proprietary Coherent Labs and/or its
licensors, which is protected by United States of America federal
copyright law and by international treaties.

This software or source code is supplied under the terms of a license
agreement and nondisclosure agreement with Coherent Labs Limited and may
not be copied, disclosed, or exploited except in accordance with the
terms of that agreement. The Data may not be disclosed or distributed to
third parties, in whole or in part, without the prior written consent of
Coherent Labs Limited.

COHERENT LABS MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS
SOURCE CODE FOR ANY PURPOSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER, ITS AFFILIATES,
PARENT COMPANIES, LICENSORS, SUPPLIERS, OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OR PERFORMANCE OF THIS SOFTWARE OR SOURCE CODE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once

#include <Coherent/UI/EventArguments.h>

#include "CoherentUIJSPayload.generated.h"
/**
* The class represents data passed by JavaScript to the application.
* Use this class to unpach parameters passed from JavaScript to the 
* application.
*/
UCLASS(MinimalAPI, BlueprintType, Blueprintable, notplaceable)
class UCoherentUIJSPayload : public UObject
{
	GENERATED_UCLASS_BODY()

public:

	/** The name of the event triggered from JavaScript */
	UPROPERTY(BlueprintReadOnly, Category="ViewScripting")
	FString EventName;

	/** Gets a String parameter passed from JavaScript
	* @param	index		the index of the parameter
	*/
	UFUNCTION(BlueprintCallable, Category = "ViewScripting")
	FString GetString(int32 index);

	/** Gets an Integer parameter passed from JavaScript
	* @param	index		the index of the parameter
	*/
	UFUNCTION(BlueprintCallable, Category = "ViewScripting")
	int32 GetInt32(int32 index);

	/** Gets an Unsigned Integer parameter passed from JavaScript
	* @param	index		the index of the parameter
	*/
#if defined(COHERENT_UI_UE4_47_SUPPORT)
	UFUNCTION(BlueprintCallable, Category = "ViewScripting")
	uint32 GetUInt32(int32 index);
#endif

	/** Gets a Boolean parameter passed from JavaScript
	* @param	index		the index of the parameter
	*/
	UFUNCTION(BlueprintCallable, Category = "ViewScripting")
	bool GetBool(int32 index);

	/** Gets a Float parameter passed from JavaScript
	* @param	index		the index of the parameter
	*/
	UFUNCTION(BlueprintCallable, Category = "ViewScripting")
	float GetNumber(int32 index);

	/** Reads an Object passed from JavaScript and updates
	* the properties of the UObject passed. All properties that have the 
	* same name and compatible type will be updated.
	* @param	index		the index of the parameter
	* @param	object		the object whose properties to update
	*/
	UFUNCTION(BlueprintCallable, Category = "ViewScripting")
	void ReadObject(int32 index, UObject* object);
	
	void SetArguments(const Coherent::UI::EventArguments* args);

private:
	const Coherent::UI::EventArguments* Arguments;
};