// Fill out your copyright notice in the Description page of Project Settings.


#include "QEWB_Style.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "Components/ComboBoxString.h"
#include "Components/Widget.h"
#include "Styling/SlateTypes.h"
#include "Styling/SlateBrush.h"
#include "Fonts/SlateFontInfo.h"

void UQEWButtonStyling::ApplyButtonStyle(UButton* Button, const FQEWButtonStyleSetup& Setup)
{
	if (!Button) return;

	FButtonStyle NewStyle = Button->WidgetStyle;

	NewStyle.Normal.TintColor = FSlateColor(Setup.NormalBg);
	NewStyle.Hovered.TintColor = FSlateColor(Setup.HoverBg);
	NewStyle.Pressed.TintColor = FSlateColor(Setup.PressedBg);
	NewStyle.Disabled.TintColor = FSlateColor(Setup.DisabledBg);

	Button->SetStyle(NewStyle);
}

void UQEWButtonStyling::BindTextTintEvents(UButton* Button, UTextBlock* LinkedText, const FQEWButtonStyleSetup& Setup)
{
	if (!Button || !LinkedText || !Setup.bTintLinkedText)
		return;

	// Set initial (normal) color
	LinkedText->SetColorAndOpacity(Setup.NormalText);

	// IMPORTANT:
	// We bind lambdas via AddWeakLambda (C++ only). This avoids needing a custom class.
	// If you need this callable purely in BP without C++, you’ll need a custom class or a BP wrapper widget.

	//Button->OnHovered.AddWeakLambda(LinkedText, [LinkedText, Setup]()
	//	{
	//		if (LinkedText) LinkedText->SetColorAndOpacity(Setup.HoverText);
	//	});

	//Button->OnUnhovered.AddWeakLambda(LinkedText, [LinkedText, Setup]()
	//	{
	//		if (LinkedText) LinkedText->SetColorAndOpacity(Setup.NormalText);
	//	});

	//Button->OnPressed.AddWeakLambda(LinkedText, [LinkedText, Setup]()
	//	{
	//		if (LinkedText) LinkedText->SetColorAndOpacity(Setup.PressedText);
	//	});

	//Button->OnReleased.AddWeakLambda(LinkedText, [LinkedText, Setup, Button]()
	//	{
	//		if (!LinkedText) return;

	//		// Return to hover if still hovered, otherwise normal
	//		if (Button && Button->IsHovered())
	//			LinkedText->SetColorAndOpacity(Setup.HoverText);
	//		else
	//			LinkedText->SetColorAndOpacity(Setup.NormalText);
	//	});
}

FSlateBrush UQEWB_ComboStyling::MakeTintBrush(const FLinearColor& Tint)
{
	FSlateBrush Brush;
	Brush.DrawAs = ESlateBrushDrawType::Box;
	Brush.TintColor = Tint;
	// These margins give you a nicer "panel" feel using default box rendering.
	Brush.Margin = FMargin(0.25f);
	return Brush;
}

void UQEWB_ComboStyling::ApplyComboBoxStyle(UComboBoxString* Combo, const FQEWB_ComboStyle& S)
{
	if (!Combo) return;

	// --- Button style (collapsed state) ---
	FComboBoxStyle NewComboStyle = Combo->WidgetStyle;

	NewComboStyle.ComboButtonStyle.ButtonStyle.Normal = MakeTintBrush(S.NormalBg);
	NewComboStyle.ComboButtonStyle.ButtonStyle.Hovered = MakeTintBrush(S.HoverBg);
	NewComboStyle.ComboButtonStyle.ButtonStyle.Pressed = MakeTintBrush(S.PressedBg);
	NewComboStyle.ComboButtonStyle.ButtonStyle.Disabled = MakeTintBrush(S.DisabledBg);

	NewComboStyle.ComboButtonStyle.ContentPadding = S.ContentPadding;

	// If you want, you can also style the "down arrow" button, but UMG’s exposure is limited.
	// Still, padding helps:
	NewComboStyle.ComboButtonStyle.DownArrowPadding = FMargin(0.f, 0.f, S.DownArrowPaddingRight, 0.f);

	// --- Row style (dropdown items) ---
	FTableRowStyle RowStyle = Combo->ItemStyle;

	RowStyle.SetEvenRowBackgroundBrush(MakeTintBrush(S.NormalBg));
	RowStyle.SetOddRowBackgroundBrush(MakeTintBrush(S.NormalBg));
	RowStyle.SetActiveBrush(MakeTintBrush(S.ItemHoverBg));
	RowStyle.SetActiveHoveredBrush(MakeTintBrush(S.ItemHoverBg));
	RowStyle.SetInactiveBrush(MakeTintBrush(S.NormalBg));
	RowStyle.SetInactiveHoveredBrush(MakeTintBrush(S.HoverBg));

	// Text colors for selection can be driven by the text widget (next section),
	// but row style also has text colors used by some list rows:
	RowStyle.SetTextColor(S.ItemTextColor);
	RowStyle.SetSelectedTextColor(S.ItemTextColor);

	Combo->WidgetStyle = NewComboStyle;
	Combo->ItemStyle = RowStyle;

	// --- Dropdown sizing ---
	Combo->MaxListHeight = S.MaxListHeight;

	// Force UMG to rebuild internal slate widgets
	Combo->SynchronizeProperties();
}