#include "QEWB_Subsystem.h"

#include "Editor.h"
#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidgetBlueprint.h"

#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/Spacer.h"
#include "Components/ComboBoxString.h"
#include "Components/SizeBox.h"

#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBoxSlot.h"

#include "Framework/Application/SlateApplication.h"
#include "Widgets/SWindow.h"

// ScriptableEditorWidgets for picker widgets
#include "Components/SinglePropertyView.h"
#include "Components/PropertyViewBase.h"

/* ------------------------------ Internal Helpers ------------------------------ */

static void AddChildFill(UPanelWidget* Parent, UWidget* Child, const FMargin& Padding = FMargin(0))
{
	if (!Parent || !Child) return;

	if (UVerticalBox* VBox = Cast<UVerticalBox>(Parent))
	{
		UVerticalBoxSlot* Slot = VBox->AddChildToVerticalBox(Child);
		Slot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		Slot->SetPadding(Padding);
		Slot->SetHorizontalAlignment(HAlign_Fill);
		Slot->SetVerticalAlignment(VAlign_Fill);
		return;
	}
	if (UHorizontalBox* HBox = Cast<UHorizontalBox>(Parent))
	{
		UHorizontalBoxSlot* Slot = HBox->AddChildToHorizontalBox(Child);
		Slot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		Slot->SetPadding(Padding);
		Slot->SetHorizontalAlignment(HAlign_Fill);
		Slot->SetVerticalAlignment(VAlign_Fill);
		return;
	}

	Parent->AddChild(Child);
}

static UPanelWidget* RequireContainer(UQEWB_WindowHandle* Handle)
{
	return Handle ? Handle->Current() : nullptr;
}

static UHorizontalBox* AddLabeledRow(UQEWB_WindowHandle* Handle, const FText& LabelText)
{
	UPanelWidget* Parent = RequireContainer(Handle);
	if (!Parent || !Handle) return nullptr;

	UHorizontalBox* Row = NewObject<UHorizontalBox>(Parent);
	AddChildFill(Parent, Row, FMargin(0, 2));

	UTextBlock* Label = NewObject<UTextBlock>(Row);
	Label->SetText(LabelText);

	UHorizontalBoxSlot* LabelSlot = Row->AddChildToHorizontalBox(Label);
	LabelSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	LabelSlot->Size.Value = Handle->LabelFill;
	LabelSlot->SetPadding(FMargin(0, 2, 8, 2));
	LabelSlot->SetHorizontalAlignment(HAlign_Fill);
	LabelSlot->SetVerticalAlignment(VAlign_Center);

	return Row;
}


/* ------------------------------ Subsystem API ------------------------------ */

UQEWB_WindowHandle* UQEWB_Subsystem::StartWindow(UEditorUtilityWidgetBlueprint* HostWidgetBlueprint, const FString& /*Title*/)
{
	if (!HostWidgetBlueprint || !GEditor) return nullptr;

	UEditorUtilitySubsystem* Subsys = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
	if (!Subsys) return nullptr;

	UEditorUtilityWidget* Spawned = Subsys->SpawnAndRegisterTab(HostWidgetBlueprint);
	if (!Spawned) return nullptr;

	UQEWB_HostWidget* Host = Cast<UQEWB_HostWidget>(Spawned);
	if (!Host || !Host->Root) return nullptr;

	UQEWB_WindowHandle* Handle = NewObject<UQEWB_WindowHandle>(GetTransientPackage());
	Handle->Window = Spawned;
	Handle->LayoutStack.Reset();
	Handle->LayoutStack.Add(Host->Root);

	return Handle;
}

UQEWB_WindowHandle* UQEWB_Subsystem::CreateModalWindow(
	TSubclassOf<UQEWB_HostWidget> HostWidgetClass,
	const FString& Title)
{
	if (!HostWidgetClass || !GEditor) return nullptr;

	UWorld* World = GEditor->GetEditorWorldContext().World();
	if (!World) return nullptr;

	// Create the UMG host
	UQEWB_HostWidget* Host = CreateWidget<UQEWB_HostWidget>(World, HostWidgetClass);
	if (!Host) return nullptr;

	// CRITICAL: ensure Root exists BEFORE we return the handle
	Host->EnsureRoot();

	// Force UMG -> Slate construction now so the widget is ready
	Host->TakeWidget();

	if (!Host->Root) return nullptr;

	// Create the window BUT do not show it yet
	TSharedRef<SWindow> WindowRef = SNew(SWindow)
		.Title(FText::FromString(Title))
		.SizingRule(ESizingRule::Autosized)
		.SupportsMaximize(false)
		.SupportsMinimize(false)
		[
			Host->TakeWidget()
		];

	// Create handle and point its layout root at Host->Root
	UQEWB_WindowHandle* Handle = NewObject<UQEWB_WindowHandle>(GetTransientPackage());
	Handle->ModalHost = Host;
	Handle->ModalWindow = WindowRef;
	Handle->LayoutStack.Reset();
	Handle->LayoutStack.Add(Host->Root);

	return Handle; // ✅ non-blocking
}

void UQEWB_Subsystem::ShowModalWindow(UQEWB_WindowHandle* Handle)
{
	if (!Handle || !Handle->ModalWindow.IsValid()) return;

	// NOTE: This blocks until closed (that's the point)
	Handle->bModalShown = true;
	FSlateApplication::Get().AddModalWindow(Handle->ModalWindow.ToSharedRef(), nullptr);
	Handle->bModalShown = false;

	// Emit "Closed" event if you want
	FQEWB_Event E;
	E.Type = EQEWB_EventType::Closed;
	E.Id = TEXT("Window");
	E.ValueType = EQEWB_ValueType::None;
	Handle->Emit(E);
}

void UQEWB_Subsystem::NormalizeLayouts(UQEWB_WindowHandle* Handle)
{
	if (Handle) Handle->NormalizeLayouts();
}

void UQEWB_Subsystem::SetLabelControlFill(UQEWB_WindowHandle* Handle, float LabelFill, float ControlFill)
{
	if (!Handle) return;
	Handle->LabelFill = FMath::Max(0.f, LabelFill);
	Handle->ControlFill = FMath::Max(0.f, ControlFill);
}

/* ------------------------------ Layout ------------------------------ */

void UQEWB_Subsystem::BeginVertical(UQEWB_WindowHandle* Handle, bool bBox)
{
	UPanelWidget* Parent = RequireContainer(Handle);
	if (!Parent || !Handle) return;

	UVerticalBox* VBox = NewObject<UVerticalBox>(Parent);

	if (bBox)
	{
		UBorder* Border = NewObject<UBorder>(Parent);
		Border->SetContent(VBox);
		AddChildFill(Parent, Border, FMargin(6));
	}
	else
	{
		AddChildFill(Parent, VBox);
	}

	Handle->Push(VBox);
}

void UQEWB_Subsystem::EndVertical(UQEWB_WindowHandle* Handle)
{
	if (Handle) Handle->Pop();
}

void UQEWB_Subsystem::BeginHorizontal(UQEWB_WindowHandle* Handle)
{
	UPanelWidget* Parent = RequireContainer(Handle);
	if (!Parent || !Handle) return;

	UHorizontalBox* HBox = NewObject<UHorizontalBox>(Parent);
	AddChildFill(Parent, HBox);

	Handle->Push(HBox);
}

void UQEWB_Subsystem::EndHorizontal(UQEWB_WindowHandle* Handle)
{
	if (Handle) Handle->Pop();
}

void UQEWB_Subsystem::BeginScrollView(UQEWB_WindowHandle* Handle)
{
	UPanelWidget* Parent = RequireContainer(Handle);
	if (!Parent || !Handle) return;

	UScrollBox* Scroll = NewObject<UScrollBox>(Parent);
	AddChildFill(Parent, Scroll);

	UVerticalBox* Body = NewObject<UVerticalBox>(Scroll);
	Scroll->AddChild(Body);

	Handle->Push(Body);
}

void UQEWB_Subsystem::EndScrollView(UQEWB_WindowHandle* Handle)
{
	if (Handle) Handle->Pop();
}

void UQEWB_Subsystem::BeginFoldout(UQEWB_WindowHandle* Handle, FName Id, const FText& HeaderText, bool bDefaultExpanded)
{
	UPanelWidget* Parent = RequireContainer(Handle);
	if (!Parent || !Handle) return;

	if (!Handle->BoolValues.Contains(Id))
	{
		Handle->BoolValues.Add(Id, bDefaultExpanded);
	}
	const bool bExpanded = Handle->BoolValues[Id];

	UVerticalBox* FoldRoot = NewObject<UVerticalBox>(Parent);
	AddChildFill(Parent, FoldRoot);

	UButton* HeaderBtn = NewObject<UButton>(FoldRoot);
	AddChildFill(FoldRoot, HeaderBtn);

	UTextBlock* HeaderLabel = NewObject<UTextBlock>(HeaderBtn);
	HeaderLabel->SetText(HeaderText);
	HeaderBtn->AddChild(HeaderLabel);

	UVerticalBox* Content = NewObject<UVerticalBox>(FoldRoot);
	AddChildFill(FoldRoot, Content, FMargin(4, 4, 0, 6));
	Content->SetVisibility(bExpanded ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

	UQEWB_FoldoutProxy* Proxy = NewObject<UQEWB_FoldoutProxy>(HeaderBtn);
	Proxy->Handle = Handle;
	Proxy->Id = Id;
	Proxy->Content = Content;
	HeaderBtn->OnClicked.AddDynamic(Proxy, &UQEWB_FoldoutProxy::Toggle);

	Handle->Push(Content);
}

void UQEWB_Subsystem::EndFoldout(UQEWB_WindowHandle* Handle)
{
	if (Handle) Handle->Pop();
}

void UQEWB_Subsystem::BeginToolbar(UQEWB_WindowHandle* Handle) { BeginHorizontal(Handle); }
void UQEWB_Subsystem::EndToolbar(UQEWB_WindowHandle* Handle) { EndHorizontal(Handle); }

void UQEWB_Subsystem::Space(UQEWB_WindowHandle* Handle, float Height)
{
	UPanelWidget* Parent = RequireContainer(Handle);
	if (!Parent) return;

	USpacer* Spacer = NewObject<USpacer>(Parent);
	Spacer->SetSize(FVector2D(1.f, Height));
	AddChildFill(Parent, Spacer);
}

void UQEWB_Subsystem::Separator(UQEWB_WindowHandle* Handle)
{
	UPanelWidget* Parent = RequireContainer(Handle);
	if (!Parent) return;

	UBorder* Line = NewObject<UBorder>(Parent);
	AddChildFill(Parent, Line, FMargin(0, 4));

	USizeBox* SB = NewObject<USizeBox>(Line);
	SB->SetHeightOverride(1.f);
	Line->SetContent(SB);
}

/* ------------------------------ Controls ------------------------------ */

void UQEWB_Subsystem::Label(UQEWB_WindowHandle* Handle, const FText& Text)
{
	UPanelWidget* Parent = RequireContainer(Handle);
	if (!Parent) return;

	UTextBlock* TB = NewObject<UTextBlock>(Parent);
	TB->SetText(Text);
	AddChildFill(Parent, TB);
}

void UQEWB_Subsystem::Button(UQEWB_WindowHandle* Handle, FName Id, const FText& Text)
{
	UPanelWidget* Parent = RequireContainer(Handle);
	if (!Parent || !Handle) return;

	UButton* Btn = NewObject<UButton>(Parent);
	AddChildFill(Parent, Btn);

	UTextBlock* TB = NewObject<UTextBlock>(Btn);
	TB->SetText(Text);
	Btn->AddChild(TB);

	UQEWB_ButtonProxy* Proxy = NewObject<UQEWB_ButtonProxy>(Btn);
	Proxy->Handle = Handle;
	Proxy->Id = Id;
	Btn->OnClicked.AddDynamic(Proxy, &UQEWB_ButtonProxy::OnClicked);
}

void UQEWB_Subsystem::Toggle(UQEWB_WindowHandle* Handle, FName Id, const FText& LabelText, bool bDefaultValue)
{
	if (!Handle) return;

	if (!Handle->BoolValues.Contains(Id))
	{
		Handle->BoolValues.Add(Id, bDefaultValue);
	}

	UHorizontalBox* Row = AddLabeledRow(Handle, LabelText);
	if (!Row) return;

	UCheckBox* CB = NewObject<UCheckBox>(Row);
	CB->SetIsChecked(Handle->BoolValues[Id]);

	UHorizontalBoxSlot* Slot = Row->AddChildToHorizontalBox(CB);
	Slot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	Slot->Size.Value = Handle->ControlFill;
	Slot->SetPadding(FMargin(0, 2, 0, 2));
	Slot->SetHorizontalAlignment(HAlign_Fill);
	Slot->SetVerticalAlignment(VAlign_Center);

	UQEWB_CheckboxProxy* Proxy = NewObject<UQEWB_CheckboxProxy>(CB);
	Proxy->Handle = Handle;
	Proxy->Id = Id;
	CB->OnCheckStateChanged.AddDynamic(Proxy, &UQEWB_CheckboxProxy::OnChanged);
}

void UQEWB_Subsystem::TextField(UQEWB_WindowHandle* Handle, FName Id, const FText& LabelText, const FString& DefaultValue)
{
	if (!Handle) return;

	if (!Handle->StringValues.Contains(Id))
	{
		Handle->StringValues.Add(Id, DefaultValue);
	}

	UHorizontalBox* Row = AddLabeledRow(Handle, LabelText);
	if (!Row) return;

	UEditableTextBox* TB = NewObject<UEditableTextBox>(Row);
	TB->SetText(FText::FromString(Handle->StringValues[Id]));

	UHorizontalBoxSlot* Slot = Row->AddChildToHorizontalBox(TB);
	Slot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	Slot->Size.Value = Handle->ControlFill;
	Slot->SetPadding(FMargin(0, 2, 0, 2));
	Slot->SetHorizontalAlignment(HAlign_Fill);
	Slot->SetVerticalAlignment(VAlign_Center);

	UQEWB_TextProxy* Proxy = NewObject<UQEWB_TextProxy>(TB);
	Proxy->Handle = Handle;
	Proxy->Id = Id;
	TB->OnTextChanged.AddDynamic(Proxy, &UQEWB_TextProxy::OnTextChanged);
}

void UQEWB_Subsystem::EnumPopup(UQEWB_WindowHandle* Handle, FName Id, const FText& LabelText, UEnum* EnumType, int32 DefaultValue)
{
	if (!Handle || !EnumType) return;

	if (!Handle->IntValues.Contains(Id))
	{
		Handle->IntValues.Add(Id, DefaultValue);
	}

	UHorizontalBox* Row = AddLabeledRow(Handle, LabelText);
	if (!Row) return;

	UComboBoxString* Combo = NewObject<UComboBoxString>(Row);

	TArray<FString> Options;
	for (int32 i = 0; i < EnumType->NumEnums(); ++i)
	{
		const FString D = EnumType->GetDisplayNameTextByIndex(i).ToString();
		Options.Add(D);
		Combo->AddOption(D);
	}

	const int32 Sel = FMath::Clamp(Handle->IntValues[Id], 0, Options.Num() - 1);
	if (Options.IsValidIndex(Sel)) Combo->SetSelectedOption(Options[Sel]);

	UHorizontalBoxSlot* Slot = Row->AddChildToHorizontalBox(Combo);
	Slot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	Slot->Size.Value = Handle->ControlFill;
	Slot->SetPadding(FMargin(0, 2, 0, 2));
	Slot->SetHorizontalAlignment(HAlign_Fill);
	Slot->SetVerticalAlignment(VAlign_Center);

	UQEWB_ComboProxy* Proxy = NewObject<UQEWB_ComboProxy>(Combo);
	Proxy->Handle = Handle;
	Proxy->Id = Id;
	Proxy->EnumType = EnumType;
	Combo->OnSelectionChanged.AddDynamic(Proxy, &UQEWB_ComboProxy::OnSelectionChanged);
}

void UQEWB_Subsystem::ObjectPicker(UQEWB_WindowHandle* Handle, FName Id, const FText& LabelText, UClass* AllowedClass, UObject* DefaultObject)
{
	if (!Handle || !AllowedClass) return;

	if (!Handle->ObjectValues.Contains(Id))
	{
		Handle->ObjectValues.Add(Id, DefaultObject);
	}

	UHorizontalBox* Row = AddLabeledRow(Handle, LabelText);
	if (!Row) return;

	UQEWB_ObjectPickerModel* Model = NewObject<UQEWB_ObjectPickerModel>(Handle);
	Model->SelectedObject = Handle->ObjectValues[Id];

	UQEWB_SinglePropertyView* View = NewObject<UQEWB_SinglePropertyView>(Row);
	View->SetObject(Model);
	View->SetPropertyName(GET_MEMBER_NAME_CHECKED(UQEWB_ObjectPickerModel, SelectedObject));

	UHorizontalBoxSlot* Slot = Row->AddChildToHorizontalBox(View);
	Slot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	Slot->Size.Value = Handle->ControlFill;
	Slot->SetPadding(FMargin(0, 2, 0, 2));
	Slot->SetHorizontalAlignment(HAlign_Fill);
	Slot->SetVerticalAlignment(VAlign_Center);

	UQEWB_PropertyViewProxy* Proxy = NewObject<UQEWB_PropertyViewProxy>(View);
	Proxy->Handle = Handle;
	Proxy->Id = Id;
	Proxy->Model = Model;
	Proxy->FilterClass = AllowedClass;
	Proxy->bIsClassPicker = false;

	View->GetOnPropertyChangedPublic().AddDynamic(Proxy, &UQEWB_PropertyViewProxy::OnPropertyChanged);
}

void UQEWB_Subsystem::ClassPicker(UQEWB_WindowHandle* Handle, FName Id, const FText& LabelText, UClass* BaseClass, UClass* DefaultClass)
{
	if (!Handle || !BaseClass) return;

	if (!Handle->ClassValues.Contains(Id))
	{
		Handle->ClassValues.Add(Id, DefaultClass ? DefaultClass : BaseClass);
	}

	UHorizontalBox* Row = AddLabeledRow(Handle, LabelText);
	if (!Row) return;

	UQEWB_ClassPickerModel* Model = NewObject<UQEWB_ClassPickerModel>(Handle);
	Model->SelectedClass = Handle->ClassValues[Id];

	UQEWB_SinglePropertyView* View = NewObject<UQEWB_SinglePropertyView>(Row);
	View->SetObject(Model);
	View->SetPropertyName(GET_MEMBER_NAME_CHECKED(UQEWB_ClassPickerModel, SelectedClass));

	UHorizontalBoxSlot* Slot = Row->AddChildToHorizontalBox(View);
	Slot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	Slot->Size.Value = Handle->ControlFill;
	Slot->SetPadding(FMargin(0, 2, 0, 2));
	Slot->SetHorizontalAlignment(HAlign_Fill);
	Slot->SetVerticalAlignment(VAlign_Center);

	UQEWB_PropertyViewProxy* Proxy = NewObject<UQEWB_PropertyViewProxy>(View);
	Proxy->Handle = Handle;
	Proxy->Id = Id;
	Proxy->Model = Model;
	Proxy->FilterClass = BaseClass;
	Proxy->bIsClassPicker = true;

	View->GetOnPropertyChangedPublic().AddDynamic(Proxy, &UQEWB_PropertyViewProxy::OnPropertyChanged);
}

/* ------------------------------ Events ------------------------------ */

void UQEWB_Subsystem::PollEvents(UQEWB_WindowHandle* Handle, bool bClear, TArray<FQEWB_Event>& OutEvents)
{
	OutEvents.Reset();
	if (!Handle) return;

	OutEvents = Handle->Events;
	if (bClear) Handle->Events.Reset();
}

bool UQEWB_Subsystem::DidChangeSinceLastPoll(UQEWB_WindowHandle* Handle, bool bReset)
{
	if (!Handle) return false;
	const bool bVal = Handle->bChangedSinceLastPoll;
	if (bReset) Handle->bChangedSinceLastPoll = false;
	return bVal;
}

/* ------------------------------ Per-property callback binding ------------------------------ */

void UQEWB_Subsystem::BindBoolChanged(UQEWB_WindowHandle* Handle, FName Id, FQEWB_BoolChanged Callback)
{
	if (!Handle) return;
	Handle->OnBoolChangedById.Add(Id, Callback);
}

void UQEWB_Subsystem::BindIntChanged(UQEWB_WindowHandle* Handle, FName Id, FQEWB_IntChanged Callback)
{
	if (!Handle) return;
	Handle->OnIntChangedById.Add(Id, Callback);
}

void UQEWB_Subsystem::BindFloatChanged(UQEWB_WindowHandle* Handle, FName Id, FQEWB_FloatChanged Callback)
{
	if (!Handle) return;
	Handle->OnFloatChangedById.Add(Id, Callback);
}

void UQEWB_Subsystem::BindStringChanged(UQEWB_WindowHandle* Handle, FName Id, FQEWB_StringChanged Callback)
{
	if (!Handle) return;
	Handle->OnStringChangedById.Add(Id, Callback);
}

void UQEWB_Subsystem::BindNameChanged(UQEWB_WindowHandle* Handle, FName Id, FQEWB_NameChanged Callback)
{
	if (!Handle) return;
	Handle->OnNameChangedById.Add(Id, Callback);
}

void UQEWB_Subsystem::BindObjectChanged(UQEWB_WindowHandle* Handle, FName Id, FQEWB_ObjectChanged Callback)
{
	if (!Handle) return;
	Handle->OnObjectChangedById.Add(Id, Callback);
}

void UQEWB_Subsystem::BindClassChanged(UQEWB_WindowHandle* Handle, FName Id, FQEWB_ClassChanged Callback)
{
	if (!Handle) return;
	Handle->OnClassChangedById.Add(Id, Callback);
}
