// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "DetailCustomizationsPrivatePCH.h"
#include "TextCustomization.h"
#include "IPropertyUtilities.h"
#include "STextPropertyEditableTextBox.h"

namespace
{
	/** Allows STextPropertyEditableTextBox to edit a property handle */
	class FEditableTextPropertyHandle : public IEditableTextProperty
	{
	public:
		FEditableTextPropertyHandle(const TSharedRef<IPropertyHandle>& InPropertyHandle, const TSharedPtr<IPropertyUtilities>& InPropertyUtilities)
			: PropertyHandle(InPropertyHandle)
			, PropertyUtilities(InPropertyUtilities)
		{
		}

		virtual bool IsMultiLineText() const override
		{
			return PropertyHandle->IsValidHandle() && PropertyHandle->GetMetaDataProperty()->GetBoolMetaData("MultiLine");
		}

		virtual bool IsPassword() const override
		{
			return PropertyHandle->IsValidHandle() && PropertyHandle->GetMetaDataProperty()->GetBoolMetaData("PasswordField");
		}

		virtual bool IsReadOnly() const override
		{
			return !PropertyHandle->IsValidHandle() || PropertyHandle->IsEditConst();
		}

		virtual bool IsDefaultValue() const override
		{
			return PropertyHandle->IsValidHandle() && !PropertyHandle->DiffersFromDefault();
		}

		virtual FText GetToolTipText() const override
		{
			return (PropertyHandle->IsValidHandle())
				? PropertyHandle->GetToolTipText()
				: FText::GetEmpty();
		}

		virtual int32 GetNumTexts() const override
		{
			return (PropertyHandle->IsValidHandle())
				? PropertyHandle->GetNumPerObjectValues() 
				: 0;
		}

		virtual FText GetText(const int32 InIndex) const override
		{
			if (PropertyHandle->IsValidHandle())
			{
				FString ObjectValue;
				if (PropertyHandle->GetPerObjectValue(InIndex, ObjectValue) == FPropertyAccess::Success)
				{
					FText TextValue;
					if (FTextStringHelper::ReadFromString(*ObjectValue, TextValue))
					{
						return TextValue;
					}
				}
			}

			return FText::GetEmpty();
		}

		virtual void SetText(const int32 InIndex, const FText& InText) override
		{
			if (PropertyHandle->IsValidHandle())
			{
				FString ObjectValue;
				if (FTextStringHelper::WriteToString(ObjectValue, InText))
				{
					PropertyHandle->SetPerObjectValue(InIndex, ObjectValue);
				}
			}
		}

		virtual void PreEdit() override
		{
			if (PropertyHandle->IsValidHandle())
			{
				PropertyHandle->NotifyPreChange();
			}
		}

		virtual void PostEdit() override
		{
			if (PropertyHandle->IsValidHandle())
			{
				PropertyHandle->NotifyPostChange();
				PropertyHandle->NotifyFinishedChangingProperties();
			}
		}

		virtual void RequestRefresh() override
		{
			if (PropertyUtilities.IsValid())
			{
				PropertyUtilities->RequestRefresh();
			}
		}

	private:
		TSharedRef<IPropertyHandle> PropertyHandle;
		TSharedPtr<IPropertyUtilities> PropertyUtilities;
	};
}

void FTextCustomization::CustomizeHeader( TSharedRef<class IPropertyHandle> InPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& PropertyTypeCustomizationUtils )
{
	TSharedRef<IEditableTextProperty> EditableTextProperty = MakeShareable(new FEditableTextPropertyHandle(InPropertyHandle, PropertyTypeCustomizationUtils.GetPropertyUtilities()));
	const bool bIsMultiLine = EditableTextProperty->IsMultiLineText();

	HeaderRow.FilterString(InPropertyHandle->GetPropertyDisplayName())
		.NameContent()
		[
			InPropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		.MinDesiredWidth(bIsMultiLine ? 250.f : 125.f)
		.MaxDesiredWidth(600.f)
		[
			SNew(STextPropertyEditableTextBox, EditableTextProperty)
				.Font(FEditorStyle::GetFontStyle("PropertyWindow.NormalFont"))
				.AutoWrapText(true)
		];
}

void FTextCustomization::CustomizeChildren( TSharedRef<class IPropertyHandle> InPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& PropertyTypeCustomizationUtils )
{

}
