#include "QEWB_InternalProxies.h"
#include "Components/VerticalBox.h"

#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/ExpandableArea.h"

#include "Styling/AppStyle.h"
#include "Styling/SlateTypes.h"
#include "QEWB_Style.h"


void UQEWB_ButtonProxy::OnClicked()
{
    if (!Handle) return;

    Handle->NotifyButtonClicked(Id);

    FQEWB_Event E;
    E.Type = EQEWB_EventType::Clicked;
    E.Id = Id;
    E.ValueType = EQEWB_ValueType::None;
    Handle->Emit(E);
}

void UQEWB_CheckboxProxy::OnChanged(bool bIsChecked)
{
    if (!Handle) return;

    Handle->BoolValues.FindOrAdd(Id) = bIsChecked;
    Handle->NotifyBoolChanged(Id, bIsChecked);

    FQEWB_Event E;
    E.Type = EQEWB_EventType::ValueChanged;
    E.Id = Id;
    E.ValueType = EQEWB_ValueType::Bool;
    E.BoolValue = bIsChecked;
    Handle->Emit(E);
}

void UQEWB_TextProxy::OnTextChanged(const FText& NewText)
{
    if (!Handle) return;

    const FString S = NewText.ToString();
    Handle->StringValues.FindOrAdd(Id) = S;
    Handle->NotifyStringChanged(Id, S);

    FQEWB_Event E;
    E.Type = EQEWB_EventType::ValueChanged;
    E.Id = Id;
    E.ValueType = EQEWB_ValueType::String;
    E.StringValue = S;
    Handle->Emit(E);
}

void UQEWB_ComboProxy::OnSelectionChanged(FString SelectedItem, ESelectInfo::Type)
{
    if (!Handle) return;

    Handle->StringValues.FindOrAdd(Id) = SelectedItem;
    Handle->NotifyStringChanged(Id, SelectedItem);

    FQEWB_Event E;
    E.Type = EQEWB_EventType::SelectionChanged;
    E.Id = Id;
    E.ValueType = EQEWB_ValueType::String;
    E.StringValue = SelectedItem;
    Handle->Emit(E);
}

void UQEWB_ExpandableAreaProxy::OnExpansionChanged(UExpandableArea* Area, bool bIsExpanded)
{
    if (!Handle) return;
    Handle->BoolValues.Add(Id, bIsExpanded);
}

void UQEWB_PropertyViewProxy::OnPropertyChanged(FName)
{
    if (!Handle || !Model)
    {
        return;
    }

    if (bIsClassPicker)
    {
        UQEWB_ClassPickerModel* ClassModel = Cast<UQEWB_ClassPickerModel>(Model);
        if (!ClassModel)
        {
            return;
        }

        UClass* Picked = ClassModel->SelectedClass;

        ClassModel->SelectedClass = Picked;
      

        Handle->ClassValues.FindOrAdd(Id) = Picked;
        Handle->NotifyClassChanged(Id, Picked);

        FQEWB_Event E;
        E.Type = EQEWB_EventType::SelectionChanged;
        E.Id = Id;
        E.ValueType = EQEWB_ValueType::Class;
        E.ClassValue = Picked;
        Handle->Emit(E);
        return;
    }

    UQEWB_ObjectPickerModel* ObjectModel = Cast<UQEWB_ObjectPickerModel>(Model);
    if (!ObjectModel)
    {
        return;
    }

    UObject* Picked = ObjectModel->SelectedObject;

    Handle->ObjectValues.FindOrAdd(Id) = Picked;
    Handle->NotifyObjectChanged(Id, Picked);

    FQEWB_Event E;
    E.Type = EQEWB_EventType::SelectionChanged;
    E.Id = Id;
    E.ValueType = EQEWB_ValueType::Object;
    E.ObjectValue = Picked;
    Handle->Emit(E);
}


UWidget* UQEWB_ComboBoxProxy::GenerateWidget(FString Item) 
{
    UObject* OuterObj = GetOuter() ? GetOuter() : GetTransientPackage();

    UHorizontalBox* Box = NewObject<UHorizontalBox>(OuterObj);

    UTextBlock* T = NewObject<UTextBlock>(Box);
    T->SetText(FText::FromString(Item));

    FQEWB_UnrealTheme::Apply(T);


    if (UHorizontalBoxSlot* Slot = Box->AddChildToHorizontalBox(T))
    {
        Slot->SetPadding(FMargin(8.f, 2.f));
        Slot->SetVerticalAlignment(VAlign_Center);
    }

    return Box;
}


void UQEWB_ComboSelectionProxy::OnSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
    if (!Handle) return;

    FQEWB_Event E;
    E.Type = EQEWB_EventType::ValueChanged;
    E.Id = Id;
    E.ValueType = EQEWB_ValueType::String;
    E.StringValue = SelectedItem;

    Handle->Emit(E);
}