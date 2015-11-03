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

#include "CoherentUIJSEvent.generated.h"

/**
* The class represents an event triggered by the game that will be processed
* in the JavaScript of the View. Use this class to pass parameters to JavaScript.
*/
UCLASS(MinimalAPI, BlueprintType, Blueprintable, notplaceable)
class UCoherentUIJSEvent : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	
	/** Adds an Integer parameter to the event */
	UFUNCTION(BlueprintCallable, Category = "ViewScripting")
	void AddInt32(int32 integer);

	/** Adds an Unsigned Integer parameter to the event */
#if defined(COHERENT_UI_UE4_47_SUPPORT)
	UFUNCTION(BlueprintCallable, Category = "ViewScripting")
#endif
	void AddUInt32(uint32 integer);

	/** Adds a String parameter to the event */
	UFUNCTION(BlueprintCallable, Category = "ViewScripting")
	void AddString(const FString& str);

	/** Adds a Float parameter to the event */
	UFUNCTION(BlueprintCallable, Category = "ViewScripting")
	void AddFloat(float fl);
	
	/** Adds a Boolean parameter to the event */
	UFUNCTION(BlueprintCallable, Category = "ViewScripting")
	void AddBool(bool b);

	/** Adds an Object parameter to the event */
	UFUNCTION(BlueprintCallable, Category = "ViewScripting")
	void AddObject(UObject* object);

	/** Clears the internal state of the event */
	void Clear();

	enum VariableType
	{
		VT_Int32,
		VT_String,
		VT_Object,
		VT_UInt32,
		VT_Float,
		VT_Bool,

		VT_Count
	};
	TArray<int32> Ints;
	TArray<int32> UInts;
	TArray<FString> Strings;
	TArray<float> Floats;
	TArray<bool> Bools;
	TArray<TWeakObjectPtr<UObject>> Objects;

	struct VariablePair
	{
		VariableType Type;
		int32 Id;
	};
	TArray<VariablePair> OrderedVariables;
};
