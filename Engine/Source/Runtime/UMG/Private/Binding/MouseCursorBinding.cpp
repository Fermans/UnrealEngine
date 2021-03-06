// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "UMGPrivatePCH.h"
#include "MouseCursorBinding.h"

#define LOCTEXT_NAMESPACE "UMG"

UMouseCursorBinding::UMouseCursorBinding()
{
}

bool UMouseCursorBinding::IsSupportedSource(UProperty* Property) const
{
	return IsSupportedDestination(Property);
}

bool UMouseCursorBinding::IsSupportedDestination(UProperty* Property) const
{
	static const FName MouseCursorEnum(TEXT("EMouseCursor"));
	
	if ( UByteProperty* ByteProperty = Cast<UByteProperty>(Property) )
	{
		if ( ByteProperty->IsEnum() )
		{
			return ByteProperty->Enum->GetFName() == MouseCursorEnum;
		}
	}

	return false;
}

EMouseCursor::Type UMouseCursorBinding::GetValue() const
{
	//SCOPE_CYCLE_COUNTER(STAT_UMGBinding);

	if ( UObject* Source = SourceObject.Get() )
	{
		uint8 Value = 0;
		if ( SourcePath.GetValue<uint8>(Source, Value) )
		{
			return static_cast<EMouseCursor::Type>( Value );
		}
	}

	return EMouseCursor::Default;
}

#undef LOCTEXT_NAMESPACE
