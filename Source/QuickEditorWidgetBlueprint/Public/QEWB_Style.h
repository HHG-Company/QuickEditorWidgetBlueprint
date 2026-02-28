#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateColor.h"
#include "QEWB_Style.generated.h"

USTRUCT(BlueprintType)
struct FQEWB_TextStyle
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 FontSize = 10;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor FontColor = FLinearColor::White;
};

USTRUCT(BlueprintType)
struct FQEWB_BackgroundStyle
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor BackgroundColor = FLinearColor::Black;
};

class UButton;
class UTextBlock;

USTRUCT(BlueprintType)
struct FQEWButtonStyleSetup
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickEditorWidget|Style")
	FLinearColor NormalBg = FLinearColor(0.14f, 0.14f, 0.14f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickEditorWidget|Style")
	FLinearColor HoverBg = FLinearColor(0.20f, 0.20f, 0.20f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickEditorWidget|Style")
	FLinearColor PressedBg = FLinearColor(0.08f, 0.08f, 0.08f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickEditorWidget|Style")
	FLinearColor DisabledBg = FLinearColor(0.10f, 0.10f, 0.10f, 0.45f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickEditorWidget|Style")
	FLinearColor NormalText = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickEditorWidget|Style")
	FLinearColor HoverText = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickEditorWidget|Style")
	FLinearColor PressedText = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickEditorWidget|Style")
	FLinearColor DisabledText = FLinearColor(0.65f, 0.65f, 0.65f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickEditorWidget|Style")
	bool bTintLinkedText = true;
};

/**
 * Styling helpers for standard UButtons inside QuickEditorWidget editor UIs.
 */
UCLASS()
class QUICKEDITORWIDGETBLUEPRINT_API UQEWButtonStyling : public UObject
{
	GENERATED_BODY()

public:
	/** Applies background tints through the button's WidgetStyle (Normal/Hovered/Pressed/Disabled). */
	UFUNCTION(BlueprintCallable, Category = "QuickEditorWidget|Style")
	static void ApplyButtonStyle(UButton* Button, const FQEWButtonStyleSetup& Setup);

	/**
	 * Optionally binds hover/press delegates to tint a linked TextBlock too.
	 * Call once (e.g., NativeConstruct). Safe to call multiple times if you remove previous bindings yourself.
	 */
	UFUNCTION(BlueprintCallable, Category = "QuickEditorWidget|Style")
	static void BindTextTintEvents(UButton* Button, UTextBlock* LinkedText, const FQEWButtonStyleSetup& Setup);
};