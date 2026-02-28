// Fill out your copyright notice in the Description page of Project Settings.


#include "QEWB_Style.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

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