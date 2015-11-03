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

#include "CoherentUIJSPayload.h"

namespace {
void ErrorParamOutOfBounds()
{
	UE_LOG(LogCoherentUI, Warning, TEXT("Trying to access inexistent JS event argument! Null value will be returned!"));
}

void ErrorInvalidType()
{
	UE_LOG(LogCoherentUI, Warning, TEXT("Type mismatch for JS event argument! Null value will be returned!"));
}

FString GetStringFromValue(const Coherent::UI::Value& value)
{
	const char* buffer = nullptr;
	size_t sz = 0;
	value.ToCharBuffer(buffer, sz);

	if (!sz)
	{
		return "";
	}

	FString returnValue;
	auto convertedLen = FUTF8ToTCHAR_Convert::ConvertedLength(buffer, sz);
	auto& charArray = returnValue.GetCharArray();
	charArray.AddZeroed(convertedLen + 1);

#if defined (COHERENT_UI_UE4_4_5_SUPPORT)
	FUTF8ToTCHAR_Convert::Convert(charArray.GetTypedData(), convertedLen, buffer, sz);
#else
	FUTF8ToTCHAR_Convert::Convert(charArray.GetData(), convertedLen, buffer, sz);
#endif

	return returnValue;
}

}

UCoherentUIJSPayload::UCoherentUIJSPayload(
#if defined (COHERENT_UI_UE4_4_5_SUPPORT)
	const FPostConstructInitializeProperties& PCIP)
#else
	const FObjectInitializer& PCIP)
#endif
	: Super(PCIP)
	, Arguments(nullptr)
{}

FString UCoherentUIJSPayload::GetString(int32 index)
{
	if (!Arguments || size_t(index) >= Arguments->GetCount())
	{
		ErrorParamOutOfBounds();
		return "";
	}
	auto value = (*Arguments)[index];
	if (value.GetType() != Coherent::UI::VT_String)
	{
		ErrorInvalidType();
		return "";
	}

	return GetStringFromValue(value);
}

int32 UCoherentUIJSPayload::GetInt32(int32 index)
{
	if (!Arguments || size_t(index) >= Arguments->GetCount())
	{
		ErrorParamOutOfBounds();
		return 0;
	}
	auto value = (*Arguments)[index];
	if (value.GetType() != Coherent::UI::VT_Integer
		&& value.GetType() != Coherent::UI::VT_UInteger)
	{
		ErrorInvalidType();
		return 0;
	}

	return value.ToInteger();
}

#if defined(COHERENT_UI_UE4_47_SUPPORT)
uint32 UCoherentUIJSPayload::GetUInt32(int32 index)
{
	if (!Arguments || size_t(index) >= Arguments->GetCount())
	{
		ErrorParamOutOfBounds();
		return 0;
	}
	auto value = (*Arguments)[index];
	if (value.GetType() != Coherent::UI::VT_Integer
		&& value.GetType() != Coherent::UI::VT_UInteger)
	{
		ErrorInvalidType();
		return 0;
	}

	return value.ToUInteger();
}
#endif
bool UCoherentUIJSPayload::GetBool(int32 index)
{
	if (!Arguments || size_t(index) >= Arguments->GetCount())
	{
		ErrorParamOutOfBounds();
		return false;
	}
	auto value = (*Arguments)[index];
	if (value.GetType() != Coherent::UI::VT_Boolean)
	{
		ErrorInvalidType();
		return false;
	}

	return value.ToBoolean();
}

float UCoherentUIJSPayload::GetNumber(int32 index)
{
	if (!Arguments || size_t(index) >= Arguments->GetCount())
	{
		ErrorParamOutOfBounds();
		return 0;
	}
	auto value = (*Arguments)[index];
	if (value.GetType() != Coherent::UI::VT_Number)
	{
		ErrorInvalidType();
		return 0;
	}

	return float(value.ToNumber());
}

void UCoherentUIJSPayload::ReadObject(int32 index, UObject* object)
{
	if (!Arguments || size_t(index) >= Arguments->GetCount())
	{
		ErrorParamOutOfBounds();
		return;
	}
	auto value = (*Arguments)[index];
	if (value.GetType() != Coherent::UI::VT_Object)
	{
		ErrorInvalidType();
		return;
	}

	auto jsObject = value.ToObject();

	auto cl = object->GetClass();
	for (TFieldIterator<UProperty> PropertyIt(cl, EFieldIteratorFlags::IncludeSuper);
		PropertyIt;
		++PropertyIt)
	{
		UProperty* Property = *PropertyIt;
		auto name = Property->GetName();

		Coherent::UI::Value propertyValue = jsObject[TCHAR_TO_ANSI(*name)];

		if (propertyValue.GetType() != Coherent::UI::VT_Null)
		{
			// uint32
			{
				UUInt32Property* propUInt = Cast<UUInt32Property>(Property);
				if (propUInt && (propertyValue.GetType() == Coherent::UI::VT_Integer
					|| propertyValue.GetType() == Coherent::UI::VT_UInteger))
				{
					auto valuePtr = propUInt->ContainerPtrToValuePtr<uint32>(object);
					switch (propertyValue.GetType())
					{
					case Coherent::UI::VT_Integer:
						*valuePtr = (uint32)propertyValue.ToInteger();
						break;
					case Coherent::UI::VT_UInteger:
						*valuePtr = (uint32)propertyValue.ToUInteger();
						break;
					default:
						break;
					}
				}
			}

			// int32
			{
				UIntProperty* propInt = Cast<UIntProperty>(Property);
				if (propInt && (propertyValue.GetType() == Coherent::UI::VT_Integer
					|| propertyValue.GetType() == Coherent::UI::VT_UInteger))
				{
					auto valuePtr = propInt->ContainerPtrToValuePtr<int32>(object);
					switch (propertyValue.GetType())
					{
					case Coherent::UI::VT_Integer:
						*valuePtr = (int32)propertyValue.ToInteger();
						break;
					case Coherent::UI::VT_UInteger:
						*valuePtr = (int32)propertyValue.ToUInteger();
						break;
					default:
						break;
					}
				}
			}

			// bool
			{
				UBoolProperty* propBool = Cast<UBoolProperty>(Property);
				if (propBool && (propertyValue.GetType() == Coherent::UI::VT_Boolean))
				{
					auto valuePtr = propBool->ContainerPtrToValuePtr<bool>(object);
					*valuePtr = propertyValue.ToBoolean();
				}
			}

			// float
			{
				UFloatProperty* propFloat = Cast<UFloatProperty>(Property);
				if (propFloat && (propertyValue.GetType() == Coherent::UI::VT_Number
					|| propertyValue.GetType() == Coherent::UI::VT_Integer
					|| propertyValue.GetType() == Coherent::UI::VT_UInteger))
				{
					auto valuePtr = propFloat->ContainerPtrToValuePtr<float>(object);
					switch (propertyValue.GetType())
					{
					case Coherent::UI::VT_Number:
						*valuePtr = (float)propertyValue.ToNumber();
						break;
					case Coherent::UI::VT_Integer:
						*valuePtr = (float)propertyValue.ToInteger();
						break;
					case Coherent::UI::VT_UInteger:
						*valuePtr = (float)propertyValue.ToUInteger();
						break;
					default:
						break;
					}
				}
			}
			// double
			{
				UDoubleProperty* propDouble = Cast<UDoubleProperty>(Property);
				if (propDouble && (propertyValue.GetType() == Coherent::UI::VT_Number
					|| propertyValue.GetType() == Coherent::UI::VT_Integer
					|| propertyValue.GetType() == Coherent::UI::VT_UInteger))
				{
					auto valuePtr = propDouble->ContainerPtrToValuePtr<double>(object);
					switch (propertyValue.GetType())
					{
					case Coherent::UI::VT_Number:
						*valuePtr = (double)propertyValue.ToNumber();
						break;
					case Coherent::UI::VT_Integer:
						*valuePtr = (double)propertyValue.ToInteger();
						break;
					case Coherent::UI::VT_UInteger:
						*valuePtr = (double)propertyValue.ToUInteger();
						break;
					default:
						break;
					}
				}
			}

			// string
			{
				UStrProperty* propStr = Cast<UStrProperty>(Property);
				if (propStr && (propertyValue.GetType() == Coherent::UI::VT_String))
				{
					auto valuePtr = propStr->ContainerPtrToValuePtr<FString>(object);
					*valuePtr = GetStringFromValue(propertyValue);
				}
			}
		}
	}

}

void UCoherentUIJSPayload::SetArguments(const Coherent::UI::EventArguments* args)
{
	Arguments = args;
}
