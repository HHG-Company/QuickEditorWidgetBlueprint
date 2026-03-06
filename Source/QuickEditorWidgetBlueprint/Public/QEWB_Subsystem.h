#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"

#include "QEWB_EventTypes.h"
#include "QEWB_WindowHandle.h"
#include "QEWB_HostWidget.h"

#include "QEWB_Subsystem.generated.h"

class UEditorUtilityWidgetBlueprint;
class UEnum;
struct FQEWB_BackgroundStyle;
struct FQEWButtonStyleSetup;
struct FQEWB_ComboStyle;

UCLASS()
class QUICKEDITORWIDGETBLUEPRINT_API UQEWB_Subsystem : public UEditorSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "QuickEditorWidgetBlueprint|Window")
    static UQEWB_WindowHandle* StartWindow(const FString& Title);

    UFUNCTION(BlueprintCallable, Category = "QuickEditorWidgetBlueprint|Window")
    static UQEWB_WindowHandle* CreateModalWindow(const FString& Title);

    UFUNCTION(BlueprintCallable, Category = "QuickEditorWidgetBlueprint|Window")
    static UQEWB_WindowHandle* StartTab(const FString& Title);

    UFUNCTION(BlueprintCallable, Category = "QuickEditorWidgetBlueprint|Window")
    static void ShowModalWindow(UQEWB_WindowHandle* Handle);

    UFUNCTION(BlueprintCallable, Category = "QuickEditorWidgetBlueprint|Window")
    static void CloseWindow(UQEWB_WindowHandle* Handle);

    UFUNCTION()
    static UQEWB_WindowHandle* NormalizeLayouts(UQEWB_WindowHandle* Handle);

    UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Settings")
    static UQEWB_WindowHandle* SetLabelControlFill(UQEWB_WindowHandle* Handle, float LabelFill, float ControlFill);

    // Layout
    UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Layout")
    static UQEWB_WindowHandle* BeginVertical(
        UQEWB_WindowHandle* Handle,
        bool bBox,
        EQEWB_SlotRule SlotRule,
        FMargin Margin
    );

    UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Layout")
    static UQEWB_WindowHandle* EndVertical(UQEWB_WindowHandle* Handle);

    UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Layout")
    static UQEWB_WindowHandle* BeginHorizontal(
        UQEWB_WindowHandle* Handle,
        EQEWB_SlotRule SlotRule,
        FMargin Margin
    );

    UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Layout")
    static UQEWB_WindowHandle* EndHorizontal(UQEWB_WindowHandle* Handle);

    UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Layout")
    static UQEWB_WindowHandle* BeginScrollView(UQEWB_WindowHandle* Handle, EQEWB_SlotRule SlotRule);

    UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Layout")
    static UQEWB_WindowHandle* EndScrollView(UQEWB_WindowHandle* Handle);

    UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Layout")
    static UQEWB_WindowHandle* BeginFoldout(UQEWB_WindowHandle* Handle, FName Id, const FText& HeaderText, bool bDefaultExpanded, EQEWB_SlotRule SlotRule);

    UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Layout")
    static UQEWB_WindowHandle* EndFoldout(UQEWB_WindowHandle* Handle);

    UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Layout")
    static UQEWB_WindowHandle* AddSpace(UQEWB_WindowHandle* Handle, float Height, EQEWB_SlotRule SlotRule);

    UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Layout")
    static UQEWB_WindowHandle* AddSeparator(UQEWB_WindowHandle* Handle, EQEWB_SlotRule SlotRule);

    // Controls
    UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Controls")
    static UQEWB_WindowHandle* AddLabel(UQEWB_WindowHandle* Handle, const FText& Text,EQEWB_SlotRule SlotRule);

    UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Controls")
    static UQEWB_WindowHandle* AddButton(UQEWB_WindowHandle* Handle,
        FName Id,
        const FText& Text,
        EQEWB_SlotRule SlotRule);

    UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Controls")
    static UQEWB_WindowHandle* AddToggle(UQEWB_WindowHandle* Handle, FName Id, const FText& LabelText, bool bDefaultValue, EQEWB_SlotRule SlotRule);

    UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Controls")
    static UQEWB_WindowHandle* AddTextField(UQEWB_WindowHandle* Handle,
        FName Id,
        const FText& LabelText,
        const FString& DefaultValue,
        EQEWB_SlotRule SlotRule);

    UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Controls")
    static UQEWB_WindowHandle* AddEnumDropdown(UQEWB_WindowHandle* Handle, FName Id, const FText& LabelText, UEnum* EnumType, int32 DefaultValue, EQEWB_SlotRule SlotRule);

    UFUNCTION(BlueprintCallable, Category = "QuickEditorWidgetBlueprint|Controls")
    static UQEWB_WindowHandle* AddStringDropdown(
        UQEWB_WindowHandle* Handle,
        FName Id,
        const FText& LabelText,
        const TArray<FString>& Options,
        const FString& DefaultValue,
        EQEWB_SlotRule SlotRule
    );

    UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Controls")
    static UQEWB_WindowHandle* AddObjectPicker(UQEWB_WindowHandle* Handle, FName Id, const FText& LabelText, UObject* DefaultObject, EQEWB_SlotRule SlotRule);

    UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Controls")
    static UQEWB_WindowHandle* AddClassPicker(UQEWB_WindowHandle* Handle, FName Id, const FText& LabelText, UClass* DefaultClass, EQEWB_SlotRule SlotRule);

    // Events
    UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Events")
    static void PollEvents(UQEWB_WindowHandle* Handle, bool bClear, TArray<FQEWB_Event>& OutEvents);

    UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Events")
    static bool DidChangeSinceLastPoll(UQEWB_WindowHandle* Handle, bool bReset);

    // Callbacks
    UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Callbacks")
    static UQEWB_WindowHandle* BindButtonClicked(UQEWB_WindowHandle* Handle, FName Id, FQEWB_ButtonClicked Callback);

    UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Callbacks")
    static UQEWB_WindowHandle* BindBoolChanged(UQEWB_WindowHandle* Handle, FName Id, FQEWB_BoolChanged Callback);

    UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Callbacks")
    static UQEWB_WindowHandle* BindIntChanged(UQEWB_WindowHandle* Handle, FName Id, FQEWB_IntChanged Callback);

    UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Callbacks")
    static UQEWB_WindowHandle* BindFloatChanged(UQEWB_WindowHandle* Handle, FName Id, FQEWB_FloatChanged Callback);

    UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Callbacks")
    static UQEWB_WindowHandle* BindStringChanged(UQEWB_WindowHandle* Handle, FName Id, FQEWB_StringChanged Callback);

    UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Callbacks")
    static UQEWB_WindowHandle* BindNameChanged(UQEWB_WindowHandle* Handle, FName Id, FQEWB_NameChanged Callback);

    UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Callbacks")
    static UQEWB_WindowHandle* BindObjectChanged(UQEWB_WindowHandle* Handle, FName Id, FQEWB_ObjectChanged Callback);

    UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Callbacks")
    static UQEWB_WindowHandle* BindClassChanged(UQEWB_WindowHandle* Handle, FName Id, FQEWB_ClassChanged Callback);

    UFUNCTION(BlueprintCallable, Category = "QuickEditorWidgetBlueprint|Callbacks")
    static UQEWB_WindowHandle* BroadcastCurrentValues(UQEWB_WindowHandle* Handle);
};
