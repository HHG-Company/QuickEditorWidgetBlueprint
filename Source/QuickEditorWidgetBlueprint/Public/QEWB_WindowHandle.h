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

    UPROPERTY(Transient) TMap<FName, bool> BoolValues;
    UPROPERTY(Transient) TMap<FName, int32> IntValues;
    UPROPERTY(Transient) TMap<FName, float> FloatValues;
    UPROPERTY(Transient) TMap<FName, FString> StringValues;
    UPROPERTY(Transient) TMap<FName, FName> NameValues;
    UPROPERTY(Transient) TMap<FName, TObjectPtr<UObject>> ObjectValues;
    UPROPERTY(Transient) TMap<FName, TObjectPtr<UClass>> ClassValues;

    // Store actual widget instances per element Id
    UPROPERTY(Transient)
    TMap<FName, TObjectPtr<UWidget>> WidgetsById;

    UPROPERTY(BlueprintAssignable, Category="QuickEditorWidgetBlueprint|Events")
    FQEWB_OnEvent OnEvent;

    UPROPERTY(Transient) TMap<FName, FQEWB_BoolChanged>   OnBoolChangedById;
    UPROPERTY(Transient) TMap<FName, FQEWB_IntChanged>    OnIntChangedById;
    UPROPERTY(Transient) TMap<FName, FQEWB_FloatChanged>  OnFloatChangedById;
    UPROPERTY(Transient) TMap<FName, FQEWB_StringChanged> OnStringChangedById;
    UPROPERTY(Transient) TMap<FName, FQEWB_NameChanged>   OnNameChangedById;
    UPROPERTY(Transient) TMap<FName, FQEWB_ObjectChanged> OnObjectChangedById;
    UPROPERTY(Transient) TMap<FName, FQEWB_ClassChanged>  OnClassChangedById;

    UPROPERTY(Transient) TMap<FName, FQEWB_ButtonClicked> OnButtonClickedById;

public:
    UPanelWidget* Current() const { return LayoutStack.Num() > 0 ? LayoutStack.Last() : nullptr; }

    void Push(UPanelWidget* Panel) { if (Panel) LayoutStack.Add(Panel); }

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
            if (D->IsBound()) { D->Execute(NewValue); }
        }
    }

    void NotifyIntChanged(FName Id, int32 NewValue)
    {
        if (FQEWB_IntChanged* D = OnIntChangedById.Find(Id))
        {
            if (D->IsBound()) { D->Execute(NewValue); }
        }
    }

    void NotifyFloatChanged(FName Id, float NewValue)
    {
        if (FQEWB_FloatChanged* D = OnFloatChangedById.Find(Id))
        {
            if (D->IsBound()) { D->Execute(NewValue); }
        }
    }

    void NotifyStringChanged(FName Id, const FString& NewValue)
    {
        if (FQEWB_StringChanged* D = OnStringChangedById.Find(Id))
        {
            if (D->IsBound()) { D->Execute(NewValue); }
        }
    }

    void NotifyNameChanged(FName Id, FName NewValue)
    {
        if (FQEWB_NameChanged* D = OnNameChangedById.Find(Id))
        {
            if (D->IsBound()) { D->Execute(NewValue); }
        }
    }

    void NotifyObjectChanged(FName Id, UObject* NewValue)
    {
        if (FQEWB_ObjectChanged* D = OnObjectChangedById.Find(Id))
        {
            if (D->IsBound()) { D->Execute(NewValue); }
        }
    }

    void NotifyClassChanged(FName Id, UClass* NewValue)
    {
        if (FQEWB_ClassChanged* D = OnClassChangedById.Find(Id))
        {
            if (D->IsBound()) { D->Execute(NewValue); }
        }
    }

    void NotifyButtonClicked(FName Id)
    {
        if (FQEWB_ButtonClicked* D = OnButtonClickedById.Find(Id))
        {
            if (D->IsBound()) { D->Execute(); }
        }
    }

    void RegisterWidget(FName Id, UWidget* Widget)
    {
        if (Widget && Id != NAME_None)
        {
            WidgetsById.Add(Id, Widget);
        }
    }
};
