#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Components/PanelWidget.h"
#include "Templates/SharedPointer.h"
#include "Widgets/SWindow.h"

#include "QEWB_EventTypes.h"
#include "QEWB_WindowHandle.generated.h"

class UEditorUtilityWidget;
class UQEWB_HostWidget;

DECLARE_DYNAMIC_DELEGATE_OneParam(FQEWB_BoolChanged, bool, NewValue);
DECLARE_DYNAMIC_DELEGATE_OneParam(FQEWB_IntChanged, int32, NewValue);
DECLARE_DYNAMIC_DELEGATE_OneParam(FQEWB_FloatChanged, float, NewValue);
DECLARE_DYNAMIC_DELEGATE_OneParam(FQEWB_StringChanged, const FString&, NewValue);
DECLARE_DYNAMIC_DELEGATE_OneParam(FQEWB_NameChanged, FName, NewValue);
DECLARE_DYNAMIC_DELEGATE_OneParam(FQEWB_ObjectChanged, UObject*, NewValue);
DECLARE_DYNAMIC_DELEGATE_OneParam(FQEWB_ClassChanged, UClass*, NewValue);

DECLARE_DYNAMIC_DELEGATE(FQEWB_ButtonClicked);

UCLASS(BlueprintType)
class QUICKEDITORWIDGETBLUEPRINT_API UQEWB_WindowHandle : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(Transient, BlueprintReadOnly, Category="QuickEditorWidgetBlueprint")
    TObjectPtr<UQEWB_HostWidget> ModalHost = nullptr;

    TSharedPtr<SWindow> ModalWindow;
    bool bModalShown = false;

    UPROPERTY()
    TObjectPtr<UQEWB_HostWidget> NonModalHost = nullptr;

    TSharedPtr<SWindow> NonModalWindow;

    UPROPERTY()
    TObjectPtr<UUserWidget> TabHost = nullptr;

    FName TabId;
    TWeakPtr<SDockTab> Tab;

    UPROPERTY(Transient)
    TArray<TObjectPtr<UPanelWidget>> LayoutStack;

    UPROPERTY(Transient)
    TArray<FQEWB_Event> Events;

    UPROPERTY(Transient)
    bool bChangedSinceLastPoll = false;

    UPROPERTY(Transient)
    float LabelFill = 0.35f;

    UPROPERTY(Transient)
    float ControlFill = 0.65f;

    UPROPERTY(Transient) 
    TMap<FName, bool> BoolValues;
    UPROPERTY(Transient) 
    TMap<FName, int32> IntValues;
    UPROPERTY(Transient) 
    TMap<FName, float> FloatValues;
    UPROPERTY(Transient) 
    TMap<FName, FString> StringValues;
    UPROPERTY(Transient) 
    TMap<FName, FName> NameValues;
    UPROPERTY(Transient) 
    TMap<FName, TObjectPtr<UObject>> ObjectValues;
    UPROPERTY(Transient) 
    TMap<FName, TObjectPtr<UClass>> ClassValues;

    UPROPERTY(Transient)
    TMap<FName, TObjectPtr<UObject>> PickerModelsById;

    UPROPERTY(Transient)
    TMap<FName, TObjectPtr<UObject>> PickerProxiesById;


    // Store actual widget instances per element Id
    UPROPERTY(Transient)
    TMap<FName, TObjectPtr<UWidget>> WidgetsById;

    UPROPERTY(BlueprintAssignable, Category="QuickEditorWidgetBlueprint|Events")
    FQEWB_OnEvent OnEvent;

    UPROPERTY(Transient) 
    TMap<FName, FQEWB_BoolChanged>   OnBoolChangedById;
    UPROPERTY(Transient) 
    TMap<FName, FQEWB_IntChanged>    OnIntChangedById;
    UPROPERTY(Transient) 
    TMap<FName, FQEWB_FloatChanged>  OnFloatChangedById;
    UPROPERTY(Transient) 
    TMap<FName, FQEWB_StringChanged> OnStringChangedById;
    UPROPERTY(Transient) 
    TMap<FName, FQEWB_NameChanged>   OnNameChangedById;
    UPROPERTY(Transient) 
    TMap<FName, FQEWB_ObjectChanged> OnObjectChangedById;
    UPROPERTY(Transient) 
    TMap<FName, FQEWB_ClassChanged>  OnClassChangedById;
    UPROPERTY(Transient) 
    TMap<FName, FQEWB_ButtonClicked> OnButtonClickedById;

    UPROPERTY()
    TArray<TObjectPtr<UObject>> OwnedUObjects;

public:
    UPanelWidget* Current() const 
    { 
        return LayoutStack.Num() > 0 ? LayoutStack.Last() : nullptr; 
    }

    void Push(UPanelWidget* Panel) 
    { 
        if (Panel) LayoutStack.Add(Panel); 
    }

    void Pop()
    {
        if (LayoutStack.Num() > 1)
        {
            LayoutStack.Pop();
        }
    }

    void NormalizeLayouts()
    {
        while (LayoutStack.Num() > 1)
        {
            LayoutStack.Pop();
        }
    }

    void Emit(const FQEWB_Event& E)
    {
        Events.Add(E);

        if (E.Type == EQEWB_EventType::ValueChanged ||
            E.Type == EQEWB_EventType::SelectionChanged ||
            E.Type == EQEWB_EventType::FoldoutChanged)
        {
            bChangedSinceLastPoll = true;
        }

        OnEvent.Broadcast(E);
    }

    void NotifyBoolChanged(FName Id, bool NewValue)
    {
        if (FQEWB_BoolChanged* D = OnBoolChangedById.Find(Id))
        {
            if (D->IsBound()) 
            { 
                D->Execute(NewValue); 
            }
        }
    }

    void NotifyIntChanged(FName Id, int32 NewValue)
    {
        if (FQEWB_IntChanged* D = OnIntChangedById.Find(Id))
        {
            if (D->IsBound()) 
            { 
                D->Execute(NewValue); 
            }
        }
    }

    void NotifyFloatChanged(FName Id, float NewValue)
    {
        if (FQEWB_FloatChanged* D = OnFloatChangedById.Find(Id))
        {
            if (D->IsBound()) 
            { 
                D->Execute(NewValue); 
            }
        }
    }

    void NotifyStringChanged(FName Id, const FString& NewValue)
    {
        if (FQEWB_StringChanged* D = OnStringChangedById.Find(Id))
        {
            if (D->IsBound()) 
            { 
                D->Execute(NewValue); 
            }
        }
    }

    void NotifyNameChanged(FName Id, FName NewValue)
    {
        if (FQEWB_NameChanged* D = OnNameChangedById.Find(Id))
        {
            if (D->IsBound()) 
            { 
                D->Execute(NewValue); 
            }
        }
    }

    void NotifyObjectChanged(FName Id, UObject* NewValue)
    {
        if (FQEWB_ObjectChanged* D = OnObjectChangedById.Find(Id))
        {
            if (D->IsBound()) 
            { 
                D->Execute(NewValue); 
            }
        }
    }

    void NotifyClassChanged(FName Id, UClass* NewValue)
    {
        if (FQEWB_ClassChanged* D = OnClassChangedById.Find(Id))
        {
            D->ExecuteIfBound(NewValue);            
        }
    }

    void NotifyButtonClicked(FName Id)
    {
        if (FQEWB_ButtonClicked* D = OnButtonClickedById.Find(Id))
        {
            D->ExecuteIfBound();            
        }
    }

    void RegisterWidget(FName Id, UWidget* Widget)
    {
        if (Widget && Id != NAME_None)
        {
            WidgetsById.Add(Id, Widget);
            NotifyAll();
        }

    }

    void NotifyAll() 
    {
        for (const TPair<FName, bool>& Pair : BoolValues)
        {
            NotifyBoolChanged(Pair.Key, Pair.Value);

            FQEWB_Event E;
            E.Type = EQEWB_EventType::ValueChanged;
            E.Id = Pair.Key;
            E.ValueType = EQEWB_ValueType::Bool;
            E.BoolValue = Pair.Value;
            Emit(E);
        }

        for (const TPair<FName, int32>& Pair : IntValues)
        {
            NotifyIntChanged(Pair.Key, Pair.Value);

            FQEWB_Event E;
            E.Type = EQEWB_EventType::ValueChanged;
            E.Id = Pair.Key;
            E.ValueType = EQEWB_ValueType::Int;
            E.IntValue = Pair.Value;
            Emit(E);
        }

        for (const TPair<FName, float>& Pair : FloatValues)
        {
            NotifyFloatChanged(Pair.Key, Pair.Value);

            FQEWB_Event E;
            E.Type = EQEWB_EventType::ValueChanged;
            E.Id = Pair.Key;
            E.ValueType = EQEWB_ValueType::Float;
            E.FloatValue = Pair.Value;
            Emit(E);
        }

        for (const TPair<FName, FString>& Pair : StringValues)
        {
            NotifyStringChanged(Pair.Key, Pair.Value);

            FQEWB_Event E;
            E.Type = EQEWB_EventType::ValueChanged;
            E.Id = Pair.Key;
            E.ValueType = EQEWB_ValueType::String;
            E.StringValue = Pair.Value;
            Emit(E);
        }

        for (const TPair<FName, FName>& Pair : NameValues)
        {
            NotifyNameChanged(Pair.Key, Pair.Value);

            FQEWB_Event E;
            E.Type = EQEWB_EventType::ValueChanged;
            E.Id = Pair.Key;
            E.ValueType = EQEWB_ValueType::Name;
            E.NameValue = Pair.Value;
            Emit(E);
        }

        for (const TPair<FName, TObjectPtr<UObject>>& Pair : ObjectValues)
        {
            NotifyObjectChanged(Pair.Key, Pair.Value);

            FQEWB_Event E;
            E.Type = EQEWB_EventType::SelectionChanged;
            E.Id = Pair.Key;
            E.ValueType = EQEWB_ValueType::Object;
            E.ObjectValue = Pair.Value;
            Emit(E);
        }

        for (const TPair<FName, TObjectPtr<UClass>>& Pair : ClassValues)
        {
            NotifyClassChanged(Pair.Key, Pair.Value);

            FQEWB_Event E;
            E.Type = EQEWB_EventType::SelectionChanged;
            E.Id = Pair.Key;
            E.ValueType = EQEWB_ValueType::Class;
            E.ClassValue = Pair.Value;
            Emit(E);
        }
    }
};
