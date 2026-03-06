#pragma once

#include "CoreMinimal.h"

#include "QEWB_Style.generated.h"

class UButton;
class UCheckBox;
class UEditableTextBox;
class UComboBoxString;
class UTextBlock;

USTRUCT()
struct FQEWB_UnrealTheme
{
	GENERATED_BODY()
	FName ButtonStyle = "Button";
	FName CheckBoxStyle = "Checkbox";
	FName EditableTextBoxStyle = "NormalEditableTextBox";
	FName ComboBoxStyle = "ComboBox";
	FName TableRowStyle = "TableView.Row";
	FName NormalFont = "NormalFont";

	static void Apply(UButton* W, const FQEWB_UnrealTheme& T = {});
	static void Apply(UCheckBox* W, const FQEWB_UnrealTheme& T = {});
	static void Apply(UEditableTextBox* W, const FQEWB_UnrealTheme& T = {});
	static void Apply(UComboBoxString* W, const FQEWB_UnrealTheme& T = {});
	static void Apply(UTextBlock* W, const FQEWB_UnrealTheme& T = {}, int32 OverrideSize = 0);
};