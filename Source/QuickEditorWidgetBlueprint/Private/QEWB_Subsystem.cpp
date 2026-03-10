#include "QEWB_Subsystem.h"

#include "QEWB_InternalProxies.h"
#include "QEWB_SinglePropertyViewEx.h"

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
#include "Components/ExpandableArea.h"

#include "Framework/Docking/TabManager.h"
#include "Widgets/Docking/SDockTab.h"
#include "Blueprint/UserWidget.h"

#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBoxSlot.h"
#include "QEWB_Style.h"

#include "Framework/Application/SlateApplication.h"
#include "Widgets/SWindow.h"

/* ------------------------------ Internal Helpers ------------------------------ */

static void AddChildSlotRule(UPanelWidget* Parent, UWidget* Child, EQEWB_SlotRule Rule, const FMargin& Padding = FMargin(0))
{
    if (!Parent || !Child) return;

    const ESlateSizeRule::Type SizeRule = (Rule == EQEWB_SlotRule::Auto) ? ESlateSizeRule::Automatic : ESlateSizeRule::Fill;

    if (UVerticalBox* VBox = Cast<UVerticalBox>(Parent))
    {
        UVerticalBoxSlot* Slot = VBox->AddChildToVerticalBox(Child);
        Slot->SetSize(FSlateChildSize(SizeRule));
        Slot->SetPadding(Padding);
        Slot->SetHorizontalAlignment(HAlign_Fill);
        Slot->SetVerticalAlignment(VAlign_Fill);
        return;
    }
    if (UHorizontalBox* HBox = Cast<UHorizontalBox>(Parent))
    {
        UHorizontalBoxSlot* Slot = HBox->AddChildToHorizontalBox(Child);
        Slot->SetSize(FSlateChildSize(SizeRule));
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

static UHorizontalBox* AddLabeledRow(UQEWB_WindowHandle* Handle, const FText& LabelText, EQEWB_SlotRule RowRule )
{
    UPanelWidget* Parent = RequireContainer(Handle);
    if (!Parent || !Handle) return nullptr;

    UHorizontalBox* Row = NewObject<UHorizontalBox>(Parent);
    AddChildSlotRule(Parent, Row, RowRule, FMargin(0, 2));

    UTextBlock* Label = NewObject<UTextBlock>(Row);
    Label->SetText(LabelText);

    FQEWB_UnrealTheme::Apply(Label);

    UHorizontalBoxSlot* LabelSlot = Row->AddChildToHorizontalBox(Label);
    LabelSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
    LabelSlot->SetPadding(FMargin(0, 2, 8, 2));
    LabelSlot->SetHorizontalAlignment(HAlign_Fill);
    LabelSlot->SetVerticalAlignment(VAlign_Center);

    return Row;
}

/* ------------------------------ Windows ------------------------------ */

UQEWB_WindowHandle* UQEWB_Subsystem::StartWindow(const FString& Title)
{
    if (!GEditor) return nullptr;

    UWorld* World = GEditor->GetEditorWorldContext().World();
    if (!World) return nullptr;

    // Create a native host widget (no blueprint)
    UQEWB_HostWidget* Host = CreateWidget<UQEWB_HostWidget>(World, UQEWB_HostWidget::StaticClass());
    if (!Host) return nullptr;

    Host->EnsureRoot();
    Host->TakeWidget();
    if (!Host->Root) return nullptr;

    TSharedRef<SWindow> WindowRef = SNew(SWindow)
        .Title(FText::FromString(Title))
        .SizingRule(ESizingRule::UserSized)
        .ClientSize(FVector2D(800, 600))
        .SupportsMaximize(false)
        .SupportsMinimize(false)
        [
            Host->TakeWidget()
        ];

    // Non-modal: AddWindow (NOT AddModalWindow)
    FSlateApplication::Get().AddWindow(WindowRef);

    UQEWB_WindowHandle* Handle = NewObject<UQEWB_WindowHandle>(GetTransientPackage());
    Handle->NonModalHost = Host;
    Handle->NonModalWindow = WindowRef;
    Handle->LayoutStack.Reset();
    Handle->LayoutStack.Add(Host->Root);

    Host->OwningHandle = Handle;

    // Optional: emit "Closed" event when window closes
    WindowRef->SetOnWindowClosed(FOnWindowClosed::CreateLambda([Handle](const TSharedRef<SWindow>&)
        {
            if (!Handle) return;

            FQEWB_Event E;
            E.Type = EQEWB_EventType::Closed;
            E.Id = TEXT("Window");
            E.ValueType = EQEWB_ValueType::None;
            Handle->Emit(E);
        }));

    Handle->LayoutStack.Add(Host->Root);
    Host->OwningHandle = Handle;

    return Handle;
}

UQEWB_WindowHandle* UQEWB_Subsystem::CreateModalWindow(const FString& Title)
{
    if (!GEditor) return nullptr;

    UWorld* World = GEditor->GetEditorWorldContext().World();
    if (!World) return nullptr;

    UQEWB_HostWidget* Host = CreateWidget<UQEWB_HostWidget>(World, UQEWB_HostWidget::StaticClass());
    if (!Host) return nullptr;

    Host->EnsureRoot();
    Host->TakeWidget();
    if (!Host->Root) return nullptr;

    TSharedRef<SWindow> WindowRef = SNew(SWindow)
        .Title(FText::FromString(Title))
        .SizingRule(ESizingRule::Autosized)
        .SupportsMaximize(false)
        .SupportsMinimize(false)
        [
            Host->TakeWidget()
        ];

    UQEWB_WindowHandle* Handle = NewObject<UQEWB_WindowHandle>(GetTransientPackage());
    Handle->ModalHost = Host;
    Handle->ModalWindow = WindowRef;
    Handle->LayoutStack.Reset();
    Handle->LayoutStack.Add(Host->Root);
    Host->OwningHandle = Handle;
    return Handle;
}

void UQEWB_Subsystem::ShowModalWindow(UQEWB_WindowHandle* Handle)
{
    if (!Handle || !Handle->ModalWindow.IsValid()) return;

    Handle->bModalShown = true;
    FSlateApplication::Get().AddModalWindow(Handle->ModalWindow.ToSharedRef(), nullptr);
    Handle->bModalShown = false;

    FQEWB_Event E;
    E.Type = EQEWB_EventType::Closed;
    E.Id = TEXT("Window");
    E.ValueType = EQEWB_ValueType::None;
    Handle->Emit(E);
}


// Spawns the dock tab and hosts your UMG widget inside it.
static TSharedRef<SDockTab> SpawnQEWBTab_Internal(
    const FSpawnTabArgs& Args,
    TWeakObjectPtr<UQEWB_WindowHandle> WeakHandle,
    TWeakObjectPtr<UQEWB_HostWidget> WeakHost,
    FText DisplayTitle
)
{
    TSharedRef<SDockTab> DockTab =
        SNew(SDockTab)
        .TabRole(ETabRole::NomadTab)
        .Label(DisplayTitle);

    if (UQEWB_HostWidget* Host = WeakHost.Get())
    {
        Host->EnsureRoot();
        DockTab->SetContent(Host->TakeWidget());
        DockTab->SetLabel(DisplayTitle); 
    }

    DockTab->SetOnTabClosed(SDockTab::FOnTabClosedCallback::CreateLambda(
        [WeakHandle](TSharedRef<SDockTab> ClosedTab)
        {
            if (UQEWB_WindowHandle* Handle = WeakHandle.Get())
            {

                if (!Handle->TabId.IsNone())
                {
                    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(Handle->TabId);
                }

                Handle->Tab.Reset();
                Handle->TabHost = nullptr;
            }
        }
    ));

    return DockTab;
}

UQEWB_WindowHandle* UQEWB_Subsystem::StartTab(const FString& Title)
{
    if (!GEditor) return nullptr;

    UWorld* World = GEditor->GetEditorWorldContext().World();
    if (!World) return nullptr;

    // Create handle
    UQEWB_WindowHandle* Handle = NewObject<UQEWB_WindowHandle>(GetTransientPackage());

    // Create your concrete host widget (NOT abstract)
    UQEWB_HostWidget* Host = CreateWidget<UQEWB_HostWidget>(World, UQEWB_HostWidget::StaticClass());
    if (!Host) return nullptr;

    Handle->TabHost = Host;

    // Unique tab ID per instance (so you can open multiple tabs if you want)
    const FGuid Guid = FGuid::NewGuid();
    Handle->TabId = *FString::Printf(TEXT("QEWB_Tab_%s"), *Guid.ToString(EGuidFormats::Digits));
    const FText DisplayTitle = FText::FromString(Title);

    const TWeakObjectPtr<UQEWB_WindowHandle> WeakHandle(Handle);
    const TWeakObjectPtr<UQEWB_HostWidget> WeakHost(Host);

    // Register the spawner
    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
        Handle->TabId,
        FOnSpawnTab::CreateLambda([WeakHandle, WeakHost, DisplayTitle](const FSpawnTabArgs& Args)
            {
                return SpawnQEWBTab_Internal(Args, WeakHandle, WeakHost, DisplayTitle);
            })
    )
        .SetDisplayName(DisplayTitle)
        .SetMenuType(ETabSpawnerMenuType::Hidden); // hide from Window menu; optional

    // Spawn/focus the tab
    TSharedPtr<SDockTab> SpawnedTab = FGlobalTabmanager::Get()->TryInvokeTab(Handle->TabId);
    if (!SpawnedTab.IsValid())
    {
        FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(Handle->TabId);
        Handle->TabHost = nullptr;
        return nullptr;
    }

    Handle->Tab = SpawnedTab;

    // Keep your layout stack consistent with window/modal
    Host->EnsureRoot();
    Handle->LayoutStack.Reset();
    Handle->LayoutStack.Add(Host->Root);

    return Handle;
}

void UQEWB_Subsystem::CloseWindow(UQEWB_WindowHandle* Handle)
{
    if (!Handle) return;

    // Modal window
    if (Handle->ModalWindow.IsValid())
    {
        Handle->ModalWindow->RequestDestroyWindow();
        Handle->ModalWindow.Reset();
        Handle->ModalHost = nullptr;
        return;
    }

    // Non-modal window
    if (Handle->NonModalWindow.IsValid())
    {
        Handle->NonModalWindow->RequestDestroyWindow();
        Handle->NonModalWindow.Reset();
        Handle->NonModalHost = nullptr;
        return;
    }

    if (Handle->Tab.IsValid())
    {
        // Close tab
        Handle->Tab.Pin()->RequestCloseTab();

        // Spawner unregister happens in OnTabClosed callback (above),
        // but you can also do it here if you want immediate cleanup.
        return;
    }
}

/* ------------------------------ Utils ------------------------------ */

UQEWB_WindowHandle* UQEWB_Subsystem::NormalizeLayouts(UQEWB_WindowHandle* Handle)
{
    if (Handle) Handle->NormalizeLayouts();

    return Handle;
}

UQEWB_WindowHandle* UQEWB_Subsystem::SetLabelControlFill(UQEWB_WindowHandle* Handle, float LabelFill, float ControlFill)
{
    if (!Handle) return Handle;
    Handle->LabelFill = FMath::Max(0.f, LabelFill);
    Handle->ControlFill = FMath::Max(0.f, ControlFill);

    return Handle;
}


/* ------------------------------ Layout ------------------------------ */

UQEWB_WindowHandle* UQEWB_Subsystem::BeginVertical(UQEWB_WindowHandle* Handle, bool bBox, EQEWB_SlotRule SlotRule, FMargin Margin)
{
    if (!Handle) return Handle;

    UPanelWidget* Parent = RequireContainer(Handle);
    if (!Parent) return Handle;

    UVerticalBox* VBox = NewObject<UVerticalBox>(Parent);

    if (bBox)
    {
        UBorder* Border = NewObject<UBorder>(Parent);
        Border->SetPadding(Margin);          
        Border->SetContent(VBox);

        AddChildSlotRule(Parent, Border, SlotRule);
    }
    else
    {
        AddChildSlotRule(Parent, VBox, SlotRule, Margin); 
    }

    Handle->Push(VBox);

    return Handle;

}

UQEWB_WindowHandle* UQEWB_Subsystem::EndVertical(UQEWB_WindowHandle* Handle)
{
    if (Handle) Handle->Pop();

    return Handle;

}

UQEWB_WindowHandle* UQEWB_Subsystem::BeginHorizontal(UQEWB_WindowHandle* Handle, EQEWB_SlotRule SlotRule, FMargin Margin)
{
    if (!Handle) return nullptr;

    UPanelWidget* Parent = RequireContainer(Handle);
    if (!Parent) return nullptr;

    UHorizontalBox* HBox = NewObject<UHorizontalBox>(Parent);

    AddChildSlotRule(Parent, HBox, SlotRule, Margin); 

    Handle->Push(HBox);

    return Handle;

}

UQEWB_WindowHandle* UQEWB_Subsystem::EndHorizontal(UQEWB_WindowHandle* Handle)
{
    if (Handle) Handle->Pop();

    return Handle;
}

UQEWB_WindowHandle* UQEWB_Subsystem::BeginScrollView(UQEWB_WindowHandle* Handle, EQEWB_SlotRule SlotRule)
{
    UPanelWidget* Parent = RequireContainer(Handle);
    if (!Parent || !Handle) return NULL;

    UScrollBox* Scroll = NewObject<UScrollBox>(Parent);
    AddChildSlotRule(Parent, Scroll, SlotRule);

    UVerticalBox* Body = NewObject<UVerticalBox>(Scroll);
    Scroll->AddChild(Body);

    Handle->Push(Body);

    return Handle;

}

UQEWB_WindowHandle* UQEWB_Subsystem::EndScrollView(UQEWB_WindowHandle* Handle)
{
    if (Handle) Handle->Pop();

    return Handle;

}

UQEWB_WindowHandle* UQEWB_Subsystem::BeginFoldout(
    UQEWB_WindowHandle* Handle,
    FName Id,
    const FText& HeaderText,
    bool bDefaultExpanded,
    EQEWB_SlotRule SlotRule
)
{
    if (!Handle) return nullptr;

    UPanelWidget* Parent = RequireContainer(Handle);
    if (!Parent) return nullptr;

    // Persist expanded state
    if (!Handle->BoolValues.Contains(Id))
    {
        Handle->BoolValues.Add(Id, bDefaultExpanded);
    }
    const bool bExpanded = Handle->BoolValues[Id];

    UExpandableArea* Area = NewObject<UExpandableArea>(Parent);

    TArray<FName> SlotNames;
    Area->GetSlotNames(SlotNames);

    UTextBlock* HeaderLabel = NewObject<UTextBlock>(Area);
    HeaderLabel->SetText(HeaderText);

    FSlateFontInfo FontInfo = FAppStyle::GetFontStyle(TEXT("NormalFont"));
    HeaderLabel->SetFont(FontInfo);

    UVerticalBox* Content = NewObject<UVerticalBox>(Area);

    Area->SetContentForSlot(FName("Header"), HeaderLabel);
    Area->SetContentForSlot(FName("Body"), Content);
    Area->SetIsExpanded(bExpanded);

    Area->SynchronizeProperties();
    AddChildSlotRule(Parent, Area, SlotRule);

    UQEWB_ExpandableAreaProxy* Proxy = NewObject<UQEWB_ExpandableAreaProxy>(Area);
    Proxy->Handle = Handle;
    Proxy->Id = Id;
    Area->OnExpansionChanged.AddDynamic(Proxy, &UQEWB_ExpandableAreaProxy::OnExpansionChanged);

    Handle->OwnedUObjects.Add(Proxy);

    // Push body container so subsequent widgets go inside
    Handle->Push(Content);

    return Handle;
}

UQEWB_WindowHandle* UQEWB_Subsystem::EndFoldout(UQEWB_WindowHandle* Handle)
{
    if (Handle) Handle->Pop();

    return Handle;

}

UQEWB_WindowHandle* UQEWB_Subsystem::AddSpace(UQEWB_WindowHandle* Handle, float Height, EQEWB_SlotRule SlotRule)
{
    UPanelWidget* Parent = RequireContainer(Handle);
    if (!Parent) return Handle;

    USpacer* Spacer = NewObject<USpacer>(Parent);
    Spacer->SetSize(FVector2D(1.f, Height));
    AddChildSlotRule(Parent, Spacer, SlotRule);

    return Handle;

}

UQEWB_WindowHandle* UQEWB_Subsystem::AddSeparator(UQEWB_WindowHandle* Handle, EQEWB_SlotRule SlotRule)
{
    UPanelWidget* Parent = RequireContainer(Handle);
    if (!Parent) return Handle;

    UBorder* Line = NewObject<UBorder>(Parent);
    AddChildSlotRule(Parent, Line, SlotRule, FMargin(0, 4));

    USizeBox* SB = NewObject<USizeBox>(Line);
    SB->SetHeightOverride(1.f);

    return Handle;

}

/* ------------------------------ Controls ------------------------------ */

UQEWB_WindowHandle* UQEWB_Subsystem::AddLabel(UQEWB_WindowHandle* Handle,
    const FText& Text,
    EQEWB_SlotRule SlotRule)
{
    UPanelWidget* Parent = RequireContainer(Handle);
    if (!Parent) return Handle;

    UTextBlock* TB = NewObject<UTextBlock>(Parent);
    TB->SetText(Text);
    AddChildSlotRule(Parent, TB, SlotRule);

    FQEWB_UnrealTheme::Apply(TB);

    return Handle;


}

UQEWB_WindowHandle* UQEWB_Subsystem::AddButton(UQEWB_WindowHandle* Handle,
    FName Id,
    const FText& Text,
    EQEWB_SlotRule SlotRule)
{
    UPanelWidget* Parent = RequireContainer(Handle);
    if (!Parent || !Handle) return NULL;

    UButton* Btn = NewObject<UButton>(Parent);
    AddChildSlotRule(Parent, Btn, SlotRule);

    UTextBlock* TB = NewObject<UTextBlock>(Btn);
    TB->SetText(Text);
    Btn->AddChild(TB);

    UQEWB_ButtonProxy* Proxy = NewObject<UQEWB_ButtonProxy>(Btn);
    Proxy->Handle = Handle;
    Proxy->Id = Id;
    Btn->OnClicked.AddDynamic(Proxy, &UQEWB_ButtonProxy::OnClicked);

    Handle->RegisterWidget(Id, (UWidget*)Btn);

    FQEWB_UnrealTheme::Apply(Btn);

    return Handle;

}

static FSlateBrush MakeBoxBrush(const FLinearColor& Tint)
{
    FSlateBrush B;
    B.DrawAs = ESlateBrushDrawType::Box;
    B.TintColor = Tint;
    B.Margin = FMargin(0.25f);
    return B;
}

static FSlateBrush MakeImageBrush(const FLinearColor& Tint)
{
    FSlateBrush B;
    B.DrawAs = ESlateBrushDrawType::Image;
    B.TintColor = Tint;
    return B;
}

UQEWB_WindowHandle* UQEWB_Subsystem::AddToggle(
    UQEWB_WindowHandle* Handle,
    FName Id,
    const FText& LabelText,
    bool bDefaultValue,
    EQEWB_SlotRule SlotRule
)
{
    if (!Handle) return nullptr;

    if (!Handle->BoolValues.Contains(Id))
    {
        Handle->BoolValues.Add(Id, bDefaultValue);
    }

    // Create a clean row manually
    UPanelWidget* Parent = Handle->LayoutStack.Num() > 0 ? Handle->LayoutStack.Last() : nullptr;
    if (!Parent) return Handle;

    UHorizontalBox* Row = NewObject<UHorizontalBox>(Parent);
    Parent->AddChild(Row);

    // -----------------------
    // CHECKBOX
    // -----------------------
    UCheckBox* CB = NewObject<UCheckBox>(Row);
    CB->SetIsChecked(Handle->BoolValues[Id]);

    UHorizontalBoxSlot* ToggleSlot = Row->AddChildToHorizontalBox(CB);
    ToggleSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
    ToggleSlot->SetPadding(FMargin(0.f, 2.f, 6.f, 2.f));
    ToggleSlot->SetHorizontalAlignment(HAlign_Left);
    ToggleSlot->SetVerticalAlignment(VAlign_Center);

    // -----------------------
    // LABEL
    // -----------------------
    UTextBlock* Label = NewObject<UTextBlock>(Row);
    Label->SetText(LabelText);
    FQEWB_UnrealTheme::Apply(Label);

    UHorizontalBoxSlot* TextSlot = Row->AddChildToHorizontalBox(Label);
    TextSlot->SetSize(FSlateChildSize(
        (SlotRule == EQEWB_SlotRule::Auto)
        ? ESlateSizeRule::Automatic
        : ESlateSizeRule::Fill
    ));
    TextSlot->SetPadding(FMargin(0.f, 2.f, 0.f, 2.f));
    TextSlot->SetHorizontalAlignment(HAlign_Left);
    TextSlot->SetVerticalAlignment(VAlign_Center);

    FQEWB_UnrealTheme::Apply(CB);

    // -----------------------
    // BIND
    // -----------------------
    UQEWB_CheckboxProxy* Proxy = NewObject<UQEWB_CheckboxProxy>(CB);
    Proxy->Handle = Handle;
    Proxy->Id = Id;

    CB->OnCheckStateChanged.AddDynamic(Proxy, &UQEWB_CheckboxProxy::OnChanged);

    Handle->OwnedUObjects.Add(Proxy);
    Handle->RegisterWidget(Id, CB);

    return Handle;
}

UQEWB_WindowHandle* UQEWB_Subsystem::AddTextField(UQEWB_WindowHandle* Handle,
    FName Id,
    const FText& LabelText,
    const FString& DefaultValue,
    EQEWB_SlotRule SlotRule)
{
    if (!Handle) return NULL;

    if (!Handle->StringValues.Contains(Id))
    {
        Handle->StringValues.Add(Id, DefaultValue);
    }

    UHorizontalBox* Row = AddLabeledRow(Handle, LabelText, SlotRule);
    if (!Row) return Handle;

    UEditableTextBox* TB = NewObject<UEditableTextBox>(Row);
    TB->SetText(FText::FromString(Handle->StringValues[Id]));

    UHorizontalBoxSlot* Slot = Row->AddChildToHorizontalBox(TB);
    Slot->SetSize(FSlateChildSize((SlotRule == EQEWB_SlotRule::Auto) ? ESlateSizeRule::Automatic : ESlateSizeRule::Fill));

    Slot->SetPadding(FMargin(0, 2, 0, 2));
    Slot->SetHorizontalAlignment(HAlign_Fill);
    Slot->SetVerticalAlignment(VAlign_Center);

    UQEWB_TextProxy* Proxy = NewObject<UQEWB_TextProxy>(TB);
    Proxy->Handle = Handle;
    Proxy->Id = Id;
    TB->OnTextChanged.AddDynamic(Proxy, &UQEWB_TextProxy::OnTextChanged);
    Handle->OwnedUObjects.Add(Proxy);

    Handle->RegisterWidget(Id, (UWidget*)TB);

    FQEWB_UnrealTheme::Apply(TB);


    return Handle;

}

UQEWB_WindowHandle* UQEWB_Subsystem::AddStringDropdown(
        UQEWB_WindowHandle* Handle,
        FName Id,
        const FText& LabelText,
        const TArray<FString>& Options,
        const FString& DefaultValue,
        EQEWB_SlotRule SlotRule
    )
{
    if (!Handle) return nullptr;

    // Store current value
    if (!Handle->StringValues.Contains(Id))
    {
        Handle->StringValues.Add(Id, DefaultValue);
    }

    UHorizontalBox* Row = AddLabeledRow(Handle, LabelText, SlotRule);
    if (!Row) return Handle;

    UComboBoxString* Combo = NewObject<UComboBoxString>(Row);
    FQEWB_UnrealTheme::Apply(Combo);
    Combo->ForegroundColor = FLinearColor(1, 1, 1, 1);

    // Populate
    Combo->ClearOptions();
    for (const FString& Opt : Options)
    {
        Combo->AddOption(Opt);
    }

    // Pick default (fallback to first option if empty/invalid)
    FString ToSelect = Handle->StringValues[Id];
    if (!Options.Contains(ToSelect) && Options.Num() > 0)
    {
        ToSelect = Options[0];
        Handle->StringValues[Id] = ToSelect;
    }

    if (!ToSelect.IsEmpty())
    {
        Combo->SetSelectedOption(ToSelect);
    }

    // Slot layout
    UHorizontalBoxSlot* Slot = Row->AddChildToHorizontalBox(Combo);
    Slot->SetSize(FSlateChildSize((SlotRule == EQEWB_SlotRule::Auto) ? ESlateSizeRule::Automatic : ESlateSizeRule::Fill));
    Slot->SetPadding(FMargin(0, 2, 0, 2));
    Slot->SetHorizontalAlignment(HAlign_Fill);
    Slot->SetVerticalAlignment(VAlign_Center);

    // ----- Bind selection changed (proxy) -----
    UQEWB_ComboProxy* Proxy = NewObject<UQEWB_ComboProxy>(Combo);
    Proxy->Handle = Handle;
    Proxy->Id = Id;
    Combo->OnSelectionChanged.AddDynamic(Proxy, &UQEWB_ComboProxy::OnSelectionChanged);

    // ----- Bind row generation (proxy) to improve the popup look -----
    UQEWB_ComboBoxProxy* RowProxy = NewObject<UQEWB_ComboBoxProxy>(Combo);


    // Keep proxies alive if needed (recommended if you have GC issues / multi-window):
    Handle->OwnedUObjects.Add(Proxy);
    Handle->OwnedUObjects.Add(RowProxy);

    Handle->RegisterWidget(Id, Combo);

    return Handle;
}

UQEWB_WindowHandle* UQEWB_Subsystem::AddEnumDropdown(UQEWB_WindowHandle* Handle, FName Id, const FText& LabelText, UEnum* EnumType, int32 DefaultValue, EQEWB_SlotRule SlotRule)
{
    if (!Handle || !EnumType) return NULL;

    if (!Handle->StringValues.Contains(Id))
    {
        Handle->StringValues.Add(Id, EnumType->GetDisplayNameTextByIndex(DefaultValue).ToString());
    }

    UHorizontalBox* Row = AddLabeledRow(Handle, LabelText, SlotRule);
    if (!Row) return Handle;

    UComboBoxString* Combo = NewObject<UComboBoxString>(Row);
    FQEWB_UnrealTheme::Apply(Combo);
    Combo->ForegroundColor = FLinearColor(1, 1, 1, 1);


    TArray<FString> Options;
    for (int32 i = 0; i < EnumType->NumEnums(); ++i)
    {
        const FString D = EnumType->GetDisplayNameTextByIndex(i).ToString();
        Options.Add(D);
        Combo->AddOption(D);
    }

    const int32 Sel = FMath::Clamp(DefaultValue, 0, Options.Num() - 1);
    if (Options.IsValidIndex(Sel)) Combo->SetSelectedOption(Options[Sel]);

    UHorizontalBoxSlot* Slot = Row->AddChildToHorizontalBox(Combo);
    Slot->SetSize(FSlateChildSize((SlotRule == EQEWB_SlotRule::Auto) ? ESlateSizeRule::Automatic : ESlateSizeRule::Fill));
    Slot->SetPadding(FMargin(0, 2, 0, 2));
    Slot->SetHorizontalAlignment(HAlign_Fill);
    Slot->SetVerticalAlignment(VAlign_Center);

    UQEWB_ComboProxy* Proxy = NewObject<UQEWB_ComboProxy>(Combo);
    Proxy->Handle = Handle;
    Proxy->Id = Id;
    Combo->OnSelectionChanged.AddDynamic(Proxy, &UQEWB_ComboProxy::OnSelectionChanged);
    Handle->OwnedUObjects.Add(Proxy);

    return Handle;

}

UQEWB_WindowHandle* UQEWB_Subsystem::AddObjectPicker(
    UQEWB_WindowHandle* Handle,
    FName Id,
    const FText& LabelText,
    UObject* DefaultObject,
    EQEWB_SlotRule SlotRule)
{
    if (!Handle )
    {
        return nullptr;
    }

    // Stored value
    if (!Handle->ObjectValues.Contains(Id))
    {
        Handle->ObjectValues.Add(Id, DefaultObject);
    }

    // Persistent model
    UQEWB_ObjectPickerModel* Model = Cast<UQEWB_ObjectPickerModel>(
        Handle->PickerModelsById.FindRef(Id));

    if (!Model)
    {
        Model = NewObject<UQEWB_ObjectPickerModel>(Handle);
        Model->SelectedObject = Handle->ObjectValues[Id];
        Handle->PickerModelsById.Add(Id, Model);
    }
    else
    {
        Model->SelectedObject = Handle->ObjectValues[Id];
    }

    UHorizontalBox* Row = AddLabeledRow(Handle, LabelText, SlotRule);
    if (!Row)
    {
        return Handle;
    }

    UQEWB_SinglePropertyViewEx* View = NewObject<UQEWB_SinglePropertyViewEx>(Row);
    View->SetObject(Model);
    View->SetPropertyName(GET_MEMBER_NAME_CHECKED(UQEWB_ObjectPickerModel, SelectedObject));

    UHorizontalBoxSlot* Slot = Row->AddChildToHorizontalBox(View);
    Slot->SetSize(FSlateChildSize(
        (SlotRule == EQEWB_SlotRule::Auto) ? ESlateSizeRule::Automatic : ESlateSizeRule::Fill));
    Slot->SetPadding(FMargin(0, 2, 0, 2));
    Slot->SetHorizontalAlignment(HAlign_Fill);
    Slot->SetVerticalAlignment(VAlign_Center);

    // Persistent proxy
    UQEWB_PropertyViewProxy* Proxy = Cast<UQEWB_PropertyViewProxy>(
        Handle->PickerProxiesById.FindRef(Id));

    if (!Proxy)
    {
        Proxy = NewObject<UQEWB_PropertyViewProxy>(Handle);
        Handle->PickerProxiesById.Add(Id, Proxy);
    }

    Proxy->Handle = Handle;
    Proxy->Id = Id;
    Proxy->Model = Model;
    Proxy->bIsClassPicker = false;

    View->GetOnPropertyChangedPublic().AddDynamic(
        Proxy,
        &UQEWB_PropertyViewProxy::OnPropertyChanged);

    Handle->RegisterWidget(Id, View);

    // Ensure stored value is always valid
    UObject* Picked = Model->SelectedObject;
    Handle->ObjectValues[Id] = Picked;
    Handle->OwnedUObjects.Add(Proxy);

    return Handle;
}

UQEWB_WindowHandle* UQEWB_Subsystem::AddClassPicker(
    UQEWB_WindowHandle* Handle,
    FName Id,
    const FText& LabelText,
    UClass* DefaultClass,
    EQEWB_SlotRule SlotRule)
{
    if (!Handle)
    {
        return nullptr;
    }

    // Stored value
    if (!Handle->ClassValues.Contains(Id))
    {
        Handle->ClassValues.Add(Id, DefaultClass);
    }

    // Persistent model
    UQEWB_ClassPickerModel* Model = Cast<UQEWB_ClassPickerModel>(
        Handle->PickerModelsById.FindRef(Id));

    if (!Model)
    {
        Model = NewObject<UQEWB_ClassPickerModel>(Handle);
        Model->SelectedClass = Handle->ClassValues[Id];
        Handle->PickerModelsById.Add(Id, Model);
    }
    else
    {
        Model->SelectedClass = Handle->ClassValues[Id];
    }

    UHorizontalBox* Row = AddLabeledRow(Handle, LabelText, SlotRule);
    if (!Row)
    {
        return Handle;
    }

    UQEWB_SinglePropertyViewEx* View = NewObject<UQEWB_SinglePropertyViewEx>(Row);
    View->SetObject(Model);
    View->SetPropertyName(GET_MEMBER_NAME_CHECKED(UQEWB_ClassPickerModel, SelectedClass));

    UHorizontalBoxSlot* Slot = Row->AddChildToHorizontalBox(View);
    Slot->SetSize(FSlateChildSize(
        (SlotRule == EQEWB_SlotRule::Auto) ? ESlateSizeRule::Automatic : ESlateSizeRule::Fill));
    Slot->SetPadding(FMargin(0, 2, 0, 2));
    Slot->SetHorizontalAlignment(HAlign_Fill);
    Slot->SetVerticalAlignment(VAlign_Center);

    // Persistent proxy
    UQEWB_PropertyViewProxy* Proxy = Cast<UQEWB_PropertyViewProxy>(
        Handle->ClassValues.FindRef(Id));

    if (!Proxy)
    {
        Proxy = NewObject<UQEWB_PropertyViewProxy>(Handle);
        Handle->PickerProxiesById.Add(Id, Proxy);
    }

    Proxy->Handle = Handle;
    Proxy->Id = Id;
    Proxy->Model = Model;
    Proxy->bIsClassPicker = true;

    View->GetOnPropertyChangedPublic().AddDynamic(
        Proxy,
        &UQEWB_PropertyViewProxy::OnPropertyChanged);

    Handle->RegisterWidget(Id, View);

    // Ensure stored value is always valid
    UClass* Picked = Model->SelectedClass;    
    Handle->ClassValues[Id] = Picked;
    Handle->OwnedUObjects.Add(Proxy);

    return Handle;
}

UQEWB_WindowHandle* UQEWB_Subsystem::AddVectorField(
    UQEWB_WindowHandle* Handle,
    FName Id,
    const FText& LabelText,
    FVector DefaultValue,
    EQEWB_SlotRule SlotRule)
{
    if (!Handle) return nullptr;

    if (!Handle->VectorValues.Contains(Id))
    {
        Handle->VectorValues.Add(Id, DefaultValue);
    }

    UHorizontalBox* Row = AddLabeledRow(Handle, LabelText, SlotRule);
    if (!Row) return Handle;

    const FVector Current = Handle->VectorValues[Id];

    UEditableTextBox* XBox = NewObject<UEditableTextBox>(Row);
    UEditableTextBox* YBox = NewObject<UEditableTextBox>(Row);
    UEditableTextBox* ZBox = NewObject<UEditableTextBox>(Row);

    XBox->SetText(FText::AsNumber(Current.X));
    YBox->SetText(FText::AsNumber(Current.Y));
    ZBox->SetText(FText::AsNumber(Current.Z));

    UQEWB_VectorProxy* Proxy = NewObject<UQEWB_VectorProxy>(Handle);
    Proxy->Handle = Handle;
    Proxy->Id = Id;
    Proxy->XBox = XBox;
    Proxy->YBox = YBox;
    Proxy->ZBox = ZBox;

    Handle->OwnedUObjects.Add(Proxy);

    auto AddVecSlot = [&](UEditableTextBox* Box)
        {
            UHorizontalBoxSlot* Slot = Row->AddChildToHorizontalBox(Box);
            Slot->SetSize(FSlateChildSize(
                (SlotRule == EQEWB_SlotRule::Auto) ? ESlateSizeRule::Automatic : ESlateSizeRule::Fill));
            Slot->SetPadding(FMargin(0, 2, 4, 2));
            Slot->SetHorizontalAlignment(HAlign_Fill);
            Slot->SetVerticalAlignment(VAlign_Center);
            FQEWB_UnrealTheme::Apply(Box);

            Box->OnTextChanged.AddDynamic(
                Proxy,
                &UQEWB_VectorProxy::OnAnyTextChanged
            );
        };   

    AddVecSlot(XBox);
    AddVecSlot(YBox);
    AddVecSlot(ZBox);

    // Register the row by Id so enable/disable/remove affects the whole vector field.
    Handle->RegisterWidget(Id, Row);

    return Handle;
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

/* ------------------------------ Callbacks ------------------------------ */

UQEWB_WindowHandle* UQEWB_Subsystem::BindButtonClicked(UQEWB_WindowHandle* Handle, FName Id, FQEWB_ButtonClicked Callback)
{
    if (!Handle) return NULL;
    Handle->OnButtonClickedById.Add(Id, Callback);
    return Handle;

}

UQEWB_WindowHandle* UQEWB_Subsystem::BindBoolChanged(UQEWB_WindowHandle* Handle, FName Id, FQEWB_BoolChanged Callback)
{
    if (!Handle) return NULL;
    Handle->OnBoolChangedById.Add(Id, Callback);
    return Handle;

}

UQEWB_WindowHandle* UQEWB_Subsystem::BindIntChanged(UQEWB_WindowHandle* Handle, FName Id, FQEWB_IntChanged Callback)
{
    if (!Handle) return NULL;
    Handle->OnIntChangedById.Add(Id, Callback);
    return Handle;

}

UQEWB_WindowHandle* UQEWB_Subsystem::BindFloatChanged(UQEWB_WindowHandle* Handle, FName Id, FQEWB_FloatChanged Callback)
{
    if (!Handle) return NULL;
    Handle->OnFloatChangedById.Add(Id, Callback);
    return Handle;

}

UQEWB_WindowHandle* UQEWB_Subsystem::BindStringChanged(UQEWB_WindowHandle* Handle, FName Id, FQEWB_StringChanged Callback)
{
    if (!Handle) return NULL;
    Handle->OnStringChangedById.Add(Id, Callback);
    return Handle;

}

UQEWB_WindowHandle* UQEWB_Subsystem::BindNameChanged(UQEWB_WindowHandle* Handle, FName Id, FQEWB_NameChanged Callback)
{
    if (!Handle) return NULL;
    Handle->OnNameChangedById.Add(Id, Callback);
    return Handle;

}

UQEWB_WindowHandle* UQEWB_Subsystem::BindObjectChanged(UQEWB_WindowHandle* Handle, FName Id, FQEWB_ObjectChanged Callback)
{
    if (!Handle) return NULL;
    Handle->OnObjectChangedById.Add(Id, Callback);
    return Handle;

}

UQEWB_WindowHandle* UQEWB_Subsystem::BindClassChanged(UQEWB_WindowHandle* Handle, FName Id, FQEWB_ClassChanged Callback)
{
    if (!Handle) return NULL;
    Handle->OnClassChangedById.Add(Id, Callback);
    return Handle;

}

UQEWB_WindowHandle* UQEWB_Subsystem::BindVectorChanged(UQEWB_WindowHandle* Handle, FName Id, FQEWB_VectorChanged Callback)
{
    if (!Handle) return NULL;
    Handle->OnVectorChangedById.Add(Id, Callback);

    return Handle;
}

UQEWB_WindowHandle* UQEWB_Subsystem::BroadcastCurrentValues(UQEWB_WindowHandle* Handle)
{
    if (!Handle)
    {
        return NULL;
    }

    Handle->NotifyAll();
    return Handle;    
}

