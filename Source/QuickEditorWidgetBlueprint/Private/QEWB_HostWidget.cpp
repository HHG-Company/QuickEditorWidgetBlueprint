#include "QEWB_HostWidget.h"
#include "QEWB_WindowHandle.h"
#include "Blueprint/WidgetTree.h"

void UQEWB_HostWidget::EnsureRoot()
{
    if (!WidgetTree)
    {
        WidgetTree = NewObject<UWidgetTree>(this, TEXT("QEWB_WidgetTree"));
    }

    if (!Root)
    {
        Root = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("QEWB_RootVBox"));
    }

    if (!WidgetTree->RootWidget)
    {
        WidgetTree->RootWidget = Root;
    }
    else
    {
        if (UVerticalBox* ExistingVBox = Cast<UVerticalBox>(WidgetTree->RootWidget))
        {
            Root = ExistingVBox;
        }
        else
        {
            UWidget* Existing = WidgetTree->RootWidget;
            WidgetTree->RootWidget = Root;
            if (Existing)
            {
                Root->AddChild(Existing);
            }
        }
    }
}

TSharedRef<SWidget> UQEWB_HostWidget::RebuildWidget()
{
    EnsureRoot();
    return Super::RebuildWidget();
}

void UQEWB_HostWidget::NativeDestruct()
{
    if (OwningHandle)
    {
        OwningHandle->Cleanup();
        OwningHandle = nullptr;
    }

    Super::NativeDestruct();
}