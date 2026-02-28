#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "Components/SinglePropertyView.h"

#include "QEWB_EventTypes.h"
#include "QEWB_WindowHandle.h"
#include "QEWB_HostWidget.h"

#include "QEWB_Subsystem.generated.h"

class UEditorUtilityWidgetBlueprint;
class UEnum;

UCLASS()
class QUICKEDITORWIDGETBLUEPRINT_API UQEWB_ButtonProxy : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY() TObjectPtr<UQEWB_WindowHandle> Handle;
	UPROPERTY() FName Id = NAME_None;

	UFUNCTION()
	void OnClicked()
	{
		if (!Handle) return;
		FQEWB_Event E;
		E.Type = EQEWB_EventType::Clicked;
		E.Id = Id;
		E.ValueType = EQEWB_ValueType::None;
		Handle->Emit(E);
	}
};

UCLASS()
class QUICKEDITORWIDGETBLUEPRINT_API UQEWB_CheckboxProxy : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY() TObjectPtr<UQEWB_WindowHandle> Handle;
	UPROPERTY() FName Id = NAME_None;

	UFUNCTION()
	void OnChanged(bool bIsChecked)
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
};

UCLASS()
class QUICKEDITORWIDGETBLUEPRINT_API UQEWB_TextProxy : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY() TObjectPtr<UQEWB_WindowHandle> Handle;
	UPROPERTY() FName Id = NAME_None;

	UFUNCTION()
	void OnTextChanged(const FText& NewText)
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
};

UCLASS()
class QUICKEDITORWIDGETBLUEPRINT_API UQEWB_ComboProxy : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY() TObjectPtr<UQEWB_WindowHandle> Handle;
	UPROPERTY() FName Id = NAME_None;
	UPROPERTY() TObjectPtr<UEnum> EnumType = nullptr;

	UFUNCTION()
	void OnSelectionChanged(FString SelectedItem, ESelectInfo::Type Type)
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
};

UCLASS()
class QUICKEDITORWIDGETBLUEPRINT_API UQEWB_FoldoutProxy : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY() TObjectPtr<UQEWB_WindowHandle> Handle;
	UPROPERTY() FName Id = NAME_None;
	UPROPERTY() TObjectPtr<UVerticalBox> Content = nullptr;

	UFUNCTION()
	void Toggle()
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
};

// Picker models
UCLASS()
class QUICKEDITORWIDGETBLUEPRINT_API UQEWB_ClassPickerModel : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Picker")
	TObjectPtr<UClass> SelectedClass = nullptr;
};

UCLASS()
class QUICKEDITORWIDGETBLUEPRINT_API UQEWB_ObjectPickerModel : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Picker")
	TObjectPtr<UObject> SelectedObject = nullptr;
};

UCLASS()
class QUICKEDITORWIDGETBLUEPRINT_API UQEWB_PropertyViewProxy : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY() TObjectPtr<UQEWB_WindowHandle> Handle;
	UPROPERTY() FName Id = NAME_None;
	UPROPERTY() TObjectPtr<UObject> Model = nullptr;
	UPROPERTY() TObjectPtr<UClass> FilterClass = nullptr;
	UPROPERTY() bool bIsClassPicker = false;

	UFUNCTION()
	void OnPropertyChanged(FName PropertyName)
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
};

/**
 * USinglePropertyView, but with a public accessor to the protected OnPropertyChanged delegate
 * (needed for C++ binding; Epic keeps it protected in UPropertyViewBase).
 */
UCLASS()
class QUICKEDITORWIDGETBLUEPRINT_API UQEWB_SinglePropertyView : public USinglePropertyView
{
	GENERATED_BODY()

public:
	// NOTE: OnPropertyChanged is declared in UPropertyViewBase as protected.
	// We can legally expose it via a subclass.
	auto& GetOnPropertyChangedPublic()
	{
		return OnPropertyChanged;
	}
};

/**
 * Editor subsystem providing Unity-like "GUILayout" calls for Editor Utility Widgets.
 * Use: Get Editor Subsystem (QEWB_Subsystem) then call BeginVertical/Controls/etc.
 */
UCLASS()
class QUICKEDITORWIDGETBLUEPRINT_API UQEWB_Subsystem : public UEditorSubsystem
{
	GENERATED_BODY()

	friend UQEWB_PropertyViewProxy;

public:
	// ---------------- Window ----------------
	UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint")
	UQEWB_WindowHandle* StartWindow(UEditorUtilityWidgetBlueprint* HostWidgetBlueprint, const FString& Title);

	UFUNCTION(BlueprintCallable, Category = "QuickEditorWidgetBlueprint")
	UQEWB_WindowHandle* CreateModalWindow(TSubclassOf<UQEWB_HostWidget> HostWidgetClass, const FString& Title);

	UFUNCTION(BlueprintCallable, Category = "QuickEditorWidgetBlueprint")
	void ShowModalWindow(UQEWB_WindowHandle* Handle);

	UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint")
	void NormalizeLayouts(UQEWB_WindowHandle* Handle);

	UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Settings")
	void SetLabelControlFill(UQEWB_WindowHandle* Handle, float LabelFill, float ControlFill);

	// ---------------- Layout ----------------
	UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Layout")
	void BeginVertical(UQEWB_WindowHandle* Handle, bool bBox);

	UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Layout")
	void EndVertical(UQEWB_WindowHandle* Handle);

	UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Layout")
	void BeginHorizontal(UQEWB_WindowHandle* Handle);

	UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Layout")
	void EndHorizontal(UQEWB_WindowHandle* Handle);

	UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Layout")
	void BeginScrollView(UQEWB_WindowHandle* Handle);

	UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Layout")
	void EndScrollView(UQEWB_WindowHandle* Handle);

	UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Layout")
	void BeginFoldout(UQEWB_WindowHandle* Handle, FName Id, const FText& HeaderText, bool bDefaultExpanded);

	UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Layout")
	void EndFoldout(UQEWB_WindowHandle* Handle);

	UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Layout")
	void BeginToolbar(UQEWB_WindowHandle* Handle);

	UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Layout")
	void EndToolbar(UQEWB_WindowHandle* Handle);

	UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Layout")
	void Space(UQEWB_WindowHandle* Handle, float Height);

	UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Layout")
	void Separator(UQEWB_WindowHandle* Handle);

	// ---------------- Controls ----------------
	UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Controls")
	void Label(UQEWB_WindowHandle* Handle, const FText& Text);

	UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Controls")
	void Button(UQEWB_WindowHandle* Handle, FName Id, const FText& Text);

	UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Controls")
	void Toggle(UQEWB_WindowHandle* Handle, FName Id, const FText& LabelText, bool bDefaultValue);

	UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Controls")
	void TextField(UQEWB_WindowHandle* Handle, FName Id, const FText& LabelText, const FString& DefaultValue);

	UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Controls")
	void EnumPopup(UQEWB_WindowHandle* Handle, FName Id, const FText& LabelText, UEnum* EnumType, int32 DefaultValue);

	UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Controls")
	void ObjectPicker(UQEWB_WindowHandle* Handle, FName Id, const FText& LabelText, UClass* AllowedClass, UObject* DefaultObject);

	UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Controls")
	void ClassPicker(UQEWB_WindowHandle* Handle, FName Id, const FText& LabelText, UClass* BaseClass, UClass* DefaultClass);

	// ---------------- Events ----------------
	UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Events")
	void PollEvents(UQEWB_WindowHandle* Handle, bool bClear, TArray<FQEWB_Event>& OutEvents);

	UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Events")
	bool DidChangeSinceLastPoll(UQEWB_WindowHandle* Handle, bool bReset);

	// ---------------- Per-property callbacks ----------------
	UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Callbacks")
	void BindBoolChanged(UQEWB_WindowHandle* Handle, FName Id, FQEWB_BoolChanged Callback);

	UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Callbacks")
	void BindIntChanged(UQEWB_WindowHandle* Handle, FName Id, FQEWB_IntChanged Callback);

	UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Callbacks")
	void BindFloatChanged(UQEWB_WindowHandle* Handle, FName Id, FQEWB_FloatChanged Callback);

	UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Callbacks")
	void BindStringChanged(UQEWB_WindowHandle* Handle, FName Id, FQEWB_StringChanged Callback);

	UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Callbacks")
	void BindNameChanged(UQEWB_WindowHandle* Handle, FName Id, FQEWB_NameChanged Callback);

	UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Callbacks")
	void BindObjectChanged(UQEWB_WindowHandle* Handle, FName Id, FQEWB_ObjectChanged Callback);

	UFUNCTION(BlueprintCallable, Category="QuickEditorWidgetBlueprint|Callbacks")
	void BindClassChanged(UQEWB_WindowHandle* Handle, FName Id, FQEWB_ClassChanged Callback);
};
