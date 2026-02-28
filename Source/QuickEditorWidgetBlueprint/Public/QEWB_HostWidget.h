#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Components/VerticalBox.h"
#include "QEWB_HostWidget.generated.h"

/**
 * Simple UMG host used as the root for dynamic layout building.
 * Create an Editor Utility Widget Blueprint inheriting from this class.
 */
UCLASS()
class QUICKEDITORWIDGETBLUEPRINT_API UQEWB_HostWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient, BlueprintReadOnly, Category="QuickEditorWidgetBlueprint")
	TObjectPtr<UVerticalBox> Root = nullptr;

	virtual TSharedRef<SWidget> RebuildWidget() override;

	// NEW
	void EnsureRoot();
};
