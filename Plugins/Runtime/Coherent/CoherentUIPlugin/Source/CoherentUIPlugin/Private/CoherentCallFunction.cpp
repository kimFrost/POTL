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

namespace Coherent
{
namespace UI
{
class UValueToPropertyConverter
{
public:
	UValueToPropertyConverter(UFunction* function, void* parameters)
		: Function(function)
		, Parameters(parameters)
		, HasConversionErrors(false)
	{
	}

	~UValueToPropertyConverter()
	{
		for (TFieldIterator<UProperty> It(Function); It && (It->PropertyFlags & (CPF_Parm | CPF_ReturnParm)) == CPF_Parm; ++It)
		{
			It->DestroyValue_InContainer(Parameters);
		}
	}

	bool ValueToProperty(const Value& Value, UProperty* Property)
	{
		const auto Count = 6;
		for (auto Index = 0; Index != Count; ++Index)
		{
			auto Converter = Converters[Index];
			if ((this->*Converter)(Value, Property))
			{
				return true;
			}
		}
		return false;
	}

	bool ConvertArguments(const EventArguments& Arguments)
	{
		auto Index = 0;
		for (TFieldIterator<UProperty> It(Function); It && (It->PropertyFlags & (CPF_Parm | CPF_ReturnParm)) == CPF_Parm; ++It, ++Index)
		{
			auto Property = *It;
			auto Value = Arguments[Index];
			if (!ValueToProperty(Value, Property))
			{
				UE_LOG(LogCoherentUI, Warning, TEXT("Can not convert parameter %s in call to %s. Using default value."), *Property->GetName(), *Function->GetName());
				It->InitializeValue_InContainer(Parameters);
			}
		}
		return !HasConversionErrors;
	}

private:
	FString GetStringFromValue(const Value& value)
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

	void SetError(const FString& PropertyName, ValueType ExpectedType, ValueType ActualType)
	{
		static const TCHAR* ValueTypeNames[] =
		{
			TEXT("Null"),
			TEXT("Boolean"),
			TEXT("Integer"),
			TEXT("UInteger"),
			TEXT("Number"),
			TEXT("String"),
			TEXT("Array"),
			TEXT("Object"),
			TEXT("Byte"),
			TEXT("UByte"),
		};
		UE_LOG(LogCoherentUI, Warning, TEXT("Wrong type in call to %s, parameter %s - got %s, expected %s"), *Function->GetName(), *PropertyName, ValueTypeNames[ActualType], ValueTypeNames[ExpectedType]);

		HasConversionErrors = true;
	}

	void ToPropertyImpl(const Value& Value, UBoolProperty* Property)
	{
		if (Value.GetType() == VT_Boolean)
		{
			Property->SetPropertyValue_InContainer(Parameters, Value.ToBoolean());
		}
		else
		{
			SetError(Property->GetName(), VT_Boolean, Value.GetType());
		}
	}

	template <typename TNumericProperty>
	void ToNumericProperty(const Value& Value, TNumericProperty* Property)
	{
		auto Type = Value.GetType();
		switch (Type)
		{
		case VT_Integer:
			Property->SetPropertyValue_InContainer(Parameters, Value.ToInteger());
			break;
		case VT_UInteger:
			Property->SetPropertyValue_InContainer(Parameters, Value.ToUInteger());
			break;
		case VT_Number:
			Property->SetPropertyValue_InContainer(Parameters, Value.ToNumber());
			break;
		case VT_Byte:
			Property->SetPropertyValue_InContainer(Parameters, Value.ToByte());
			break;
		case VT_UByte:
			Property->SetPropertyValue_InContainer(Parameters, Value.ToUByte());
			break;
		default:
			SetError(Property->GetName(), VT_Number, Type);
			break;
	}
	}

	void ToPropertyImpl(const Value& Value, UByteProperty* Property)
	{
		ToNumericProperty(Value, Property);
	}

	void ToPropertyImpl(const Value& Value, UIntProperty* Property)
	{
		ToNumericProperty(Value, Property);
	}

	void ToPropertyImpl(const Value& Value, UFloatProperty* Property)
	{
		ToNumericProperty(Value, Property);
	}

	void ToPropertyImpl(const Value& Value, UDoubleProperty* Property)
	{
		ToNumericProperty(Value, Property);
	}

	void ToPropertyImpl(const Value& Value, UStrProperty* Property)
	{
		if (Value.GetType() == VT_String)
		{
			Property->SetPropertyValue_InContainer(Parameters, GetStringFromValue(Value));
		}
		else
		{
			SetError(Property->GetName(), VT_String, Value.GetType());
		}
	}

	template <typename TProperty>
	bool ToProperty(const Value& Value, UProperty* Property)
	{
		auto RealProperty = Cast<TProperty>(Property);
		if (RealProperty)
		{
			ToPropertyImpl(Value, RealProperty);
			return true;
		}
		return false;
	}

	typedef bool (UValueToPropertyConverter::*ToPropertyConvertor)(const Value& Value, UProperty* Property);

	static const ToPropertyConvertor Converters[];

	UFunction* Function;
	void* Parameters;
	bool HasConversionErrors;
};

const UValueToPropertyConverter::ToPropertyConvertor UValueToPropertyConverter::Converters[] = {
	&UValueToPropertyConverter::ToProperty<UBoolProperty>,
	&UValueToPropertyConverter::ToProperty<UIntProperty>,
	&UValueToPropertyConverter::ToProperty<UFloatProperty>,
	&UValueToPropertyConverter::ToProperty<UDoubleProperty>,
	&UValueToPropertyConverter::ToProperty<UStrProperty>,
	&UValueToPropertyConverter::ToProperty<UByteProperty>,
};

bool CallFunction(AActor* Actor, UFunction* Function, const EventArguments& Arguments)
{
	check(Actor);
	check(Function);

	if (Function->NumParms)
	{
		uint8* Parameters = (uint8*)FMemory_Alloca(Function->ParmsSize);
		FMemory::Memzero(Parameters, Function->ParmsSize);

		UValueToPropertyConverter Converter(Function, Parameters);

		if (Converter.ConvertArguments(Arguments))
		{
			Actor->ProcessEvent(Function, Parameters);
		}
	}
	else
	{
		Actor->ProcessEvent(Function, nullptr);
	}
	return true;
}
}
}

