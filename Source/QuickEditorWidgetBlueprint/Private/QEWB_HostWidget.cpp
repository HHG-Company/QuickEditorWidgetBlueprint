#include "QEWB_HostWidget.h"
#include "Blueprint/WidgetTree.h"

TSharedRef<SWidget> UQEWB_HostWidget::RebuildWidget()
{
	// Ensure we have a WidgetTree and set a root UMG widget before Slate is rebuilt.
	EnsureRoot();

	return Super::RebuildWidget();
}



void UQEWB_HostWidget::EnsureRoot()
{
	// Make sure WidgetTree exists (it may be null for non-blueprint-created instances)
	if (!WidgetTree)
	{
		WidgetTree = NewObject<UWidgetTree>(this, TEXT("QEWB_WidgetTree"));
	}

	// Create Root if missing, or if RootWidget isn't set
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
		// If there is a root widget but it's not our Root, try to reuse it if compatible
		if (UVerticalBox* ExistingVBox = Cast<UVerticalBox>(WidgetTree->RootWidget))
		{
			Root = ExistingVBox;
		}
		else
		{
			// Wrap existing root into a vertical box so we always have a container
			UWidget* Existing = WidgetTree->RootWidget;
			WidgetTree->RootWidget = Root;

			if (Existing)
			{
				Root->AddChild(Existing);
			}
		}
	}
}
