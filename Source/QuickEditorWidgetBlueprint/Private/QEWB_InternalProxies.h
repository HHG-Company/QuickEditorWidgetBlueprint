#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "QEWB_WindowHandle.h"
#include "Components/CheckBox.h"
#include "QEWB_InternalProxies.generated.h"

class UVerticalBox;
class UEnum;
class UExpandableArea;

UCLASS()
class UQEWB_ButtonProxy : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY() TObjectPtr<UQEWB_WindowHandle> Handle;
    UPROPERTY() FName Id = NAME_None;

    UFUNCTION() void OnClicked();
};

UCLASS()
class UQEWB_CheckboxProxy : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY() TObjectPtr<UQEWB_WindowHandle> Handle;
    UPROPERTY() FName Id = NAME_None;

    UFUNCTION() void OnChanged(bool bIsChecked);
};

UCLASS()
class UQEWB_TextProxy : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY() TObjectPtr<UQEWB_WindowHandle> Handle;
    UPROPERTY() FName Id = NAME_None;

    UFUNCTION() void OnTextChanged(const FText& NewText);
};

UCLASS()
class UQEWB_ComboProxy : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY() TObjectPtr<UQEWB_WindowHandle> Handle;
    UPROPERTY() FName Id = NAME_None;
    UPROPERTY() TObjectPtr<UEnum> EnumType = nullptr;

    UFUNCTION() void OnSelectionChanged(FString SelectedItem, ESelectInfo::Type Type);
};

UCLASS()
class UQEWB_ExpandableAreaProxy : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY() TObjectPtr<UQEWB_WindowHandle> Handle = nullptr;
    UPROPERTY() FName Id;

    UFUNCTION()
    void OnExpansionChanged(UExpandableArea* Area, bool bIsExpanded);
};

UCLASS()
class UQEWB_ClassPickerModel : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, Category="Picker")
    TObjectPtr<UClass> SelectedClass = nullptr;
};

UCLASS()
class UQEWB_ObjectPickerModel : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, Category="Picker")
    TObjectPtr<UObject> SelectedObject = nullptr;
};

UCLASS()
class UQEWB_PropertyViewProxy : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY() TObjectPtr<UQEWB_WindowHandle> Handle;
    UPROPERTY() FName Id = NAME_None;
    UPROPERTY() TObjectPtr<UObject> Model = nullptr;
    UPROPERTY() TObjectPtr<UClass> FilterClass = nullptr;
    UPROPERTY() bool bIsClassPicker = false;

    UFUNCTION() void OnPropertyChanged(FName PropertyName);
};

UCLASS()
class QUICKEDITORWIDGETBLUEPRINT_API UQEWB_ComboBoxProxy : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY() FLinearColor ItemTextColor = FLinearColor::White;
    UPROPERTY() FMargin ItemPadding = FMargin(10.f, 4.f);

    UFUNCTION()
    UWidget* GenerateWidget(FString Item);
};

UCLASS()
class QUICKEDITORWIDGETBLUEPRINT_API UQEWB_ComboSelectionProxy : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY() TObjectPtr<UQEWB_WindowHandle> Handle = nullptr;
    UPROPERTY() FName Id;

    UFUNCTION()
    void OnSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
};

UCLASS()
class QUICKEDITORWIDGETBLUEPRINT_API UQEWB_ToggleRowProxy : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY() TObjectPtr<UCheckBox> CheckBox = nullptr;

    UFUNCTION()
    void OnRowClicked()
    {
        if (!CheckBox) return;
        CheckBox->SetIsChecked(!CheckBox->IsChecked());
    }
};