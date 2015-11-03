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
#include "CoherentUIPluginPrivatePCH.h"

#include "CoherentUIJSEvent.h"

UCoherentUIJSEvent::UCoherentUIJSEvent(
#if defined (COHERENT_UI_UE4_4_5_SUPPORT)
	const FPostConstructInitializeProperties& PCIP)
#else
	const FObjectInitializer& PCIP)
#endif
	: Super(PCIP)
{}

void UCoherentUIJSEvent::AddInt32(int32 integer)
{
	Ints.Add(integer);
	VariablePair pair;
	pair.Type = VT_Int32;
	pair.Id = Ints.Num() - 1;
	OrderedVariables.Add(pair);
}

void UCoherentUIJSEvent::AddUInt32(uint32 integer)
{
	UInts.Add(integer);
	VariablePair pair;
	pair.Type = VT_UInt32;
	pair.Id = UInts.Num() - 1;
	OrderedVariables.Add(pair);
}

void UCoherentUIJSEvent::AddFloat(float fl)
{
	Floats.Add(fl);
	VariablePair pair;
	pair.Type = VT_Float;
	pair.Id = Floats.Num() - 1;
	OrderedVariables.Add(pair);
}

void UCoherentUIJSEvent::AddBool(bool b)
{
	Bools.Add(b);
	VariablePair pair;
	pair.Type = VT_Bool;
	pair.Id = Bools.Num() - 1;
	OrderedVariables.Add(pair);
}

void UCoherentUIJSEvent::AddString(const FString& str)
{
	Strings.Add(str);
	VariablePair pair;
	pair.Type = VT_String;
	pair.Id = Strings.Num() - 1;
	OrderedVariables.Add(pair);
}

void UCoherentUIJSEvent::AddObject(UObject* object)
{
	Objects.Add(object);
	VariablePair pair;
	pair.Type = VT_Object;
	pair.Id = Objects.Num() - 1;
	OrderedVariables.Add(pair);
}

void UCoherentUIJSEvent::Clear()
{
	Ints.Reset();
	UInts.Reset();
	Strings.Reset();
	Floats.Reset();
	Bools.Reset();
	Objects.Reset();
	OrderedVariables.Reset();
}
