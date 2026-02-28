#include "QEWB_InternalProxies.h"
#include "Components/VerticalBox.h"

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
    if (!Handle || !EnumType) return;

    int32 FoundIndex = INDEX_NONE;
    for (int32 i = 0; i < EnumType->NumEnums(); ++i)
    {
        if (EnumType->GetDisplayNameTextByIndex(i).ToString() == SelectedItem)
        {
            FoundIndex = i;
            break;
        }
    }
    if (FoundIndex == INDEX_NONE) return;

    Handle->IntValues.FindOrAdd(Id) = FoundIndex;
    Handle->NotifyIntChanged(Id, FoundIndex);

    FQEWB_Event E;
    E.Type = EQEWB_EventType::SelectionChanged;
    E.Id = Id;
    E.ValueType = EQEWB_ValueType::Int;
    E.IntValue = FoundIndex;
    Handle->Emit(E);
}

void UQEWB_FoldoutProxy::Toggle()
{
    if (!Handle || !Content) return;

    const bool bNowExpanded = (Content->GetVisibility() != ESlateVisibility::Visible);
    Content->SetVisibility(bNowExpanded ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

    Handle->BoolValues.FindOrAdd(Id) = bNowExpanded;
    Handle->NotifyBoolChanged(Id, bNowExpanded);

    FQEWB_Event E;
    E.Type = EQEWB_EventType::FoldoutChanged;
    E.Id = Id;
    E.ValueType = EQEWB_ValueType::Bool;
    E.BoolValue = bNowExpanded;
    Handle->Emit(E);
}

void UQEWB_PropertyViewProxy::OnPropertyChanged(FName)
{
    if (!Handle || !Model) return;

    if (bIsClassPicker)
    {
        UQEWB_ClassPickerModel* M = Cast<UQEWB_ClassPickerModel>(Model);
        if (!M) return;

        if (FilterClass && M->SelectedClass && !M->SelectedClass->IsChildOf(FilterClass))
        {
            M->SelectedClass = FilterClass;
        }

        Handle->ClassValues.FindOrAdd(Id) = M->SelectedClass;
        Handle->NotifyClassChanged(Id, M->SelectedClass);

        FQEWB_Event E;
        E.Type = EQEWB_EventType::SelectionChanged;
        E.Id = Id;
        E.ValueType = EQEWB_ValueType::Class;
        E.ClassValue = M->SelectedClass;
        Handle->Emit(E);
        return;
    }

    UQEWB_ObjectPickerModel* OM = Cast<UQEWB_ObjectPickerModel>(Model);
    if (!OM) return;

    if (FilterClass && OM->SelectedObject && !OM->SelectedObject->IsA(FilterClass))
    {
        OM->SelectedObject = nullptr;
    }

    Handle->ObjectValues.FindOrAdd(Id) = OM->SelectedObject;
    Handle->NotifyObjectChanged(Id, OM->SelectedObject);

    FQEWB_Event E;
    E.Type = EQEWB_EventType::SelectionChanged;
    E.Id = Id;
    E.ValueType = EQEWB_ValueType::Object;
    E.ObjectValue = OM->SelectedObject;
    Handle->Emit(E);
}
