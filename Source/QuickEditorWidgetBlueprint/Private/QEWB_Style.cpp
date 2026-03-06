// Fill out your copyright notice in the Description page of Project Settings.


#include "QEWB_Style.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "Components/ComboBoxString.h"
#include "Components/Widget.h"
#include "Styling/SlateTypes.h"
#include "Styling/SlateBrush.h"
#include "Fonts/SlateFontInfo.h"

#include "Styling/AppStyle.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"

void FQEWB_UnrealTheme::Apply(UButton* W, const FQEWB_UnrealTheme& T)
{
	if (!W) return;
	const FButtonStyle& S = FAppStyle::Get().GetWidgetStyle<FButtonStyle>(T.ButtonStyle);
	W->SetStyle(S);
	W->SynchronizeProperties();
}

void FQEWB_UnrealTheme::Apply(UCheckBox* W, const FQEWB_UnrealTheme& T)
{
	if (!W) return;
	const FCheckBoxStyle& S = FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>(T.CheckBoxStyle);
	W->SetWidgetStyle(S);
	W->SynchronizeProperties();
}

void FQEWB_UnrealTheme::Apply(UEditableTextBox* W, const FQEWB_UnrealTheme& T)
{
	if (!W) return;
	const FEditableTextBoxStyle& S = FAppStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>(T.EditableTextBoxStyle);
	W->WidgetStyle = S;
	W->SynchronizeProperties();
}

void FQEWB_UnrealTheme::Apply(UComboBoxString* W, const FQEWB_UnrealTheme& T)
{
	if (!W) return;

	const FComboBoxStyle& Combo = FAppStyle::Get().GetWidgetStyle<FComboBoxStyle>(T.ComboBoxStyle);
	const FTableRowStyle& Row = FAppStyle::Get().GetWidgetStyle<FTableRowStyle>(T.TableRowStyle);

	W->WidgetStyle = Combo;
	W->ItemStyle = Row;
	W->SynchronizeProperties();
}

void FQEWB_UnrealTheme::Apply(UTextBlock* W, const FQEWB_UnrealTheme& T, int32 OverrideSize)
{
	if (!W) return;

	FSlateFontInfo Font = FAppStyle::GetFontStyle(T.NormalFont);
	if (OverrideSize > 0)
	{
		Font.Size = OverrideSize;
	}
	W->SetFont(Font);

	// Let the editor default decide color unless you override explicitly
	W->SynchronizeProperties();
}
