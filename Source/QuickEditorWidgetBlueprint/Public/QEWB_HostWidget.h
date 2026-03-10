#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Components/VerticalBox.h"
#include "QEWB_HostWidget.generated.h"

UCLASS()
class QUICKEDITORWIDGETBLUEPRINT_API UQEWB_HostWidget : public UEditorUtilityWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(Transient, BlueprintReadOnly, Category="QuickEditorWidgetBlueprint")
    TObjectPtr<UVerticalBox> Root = nullptr;

    virtual TSharedRef<SWidget> RebuildWidget() override;

    void EnsureRoot();

	UPROPERTY(Transient)
	TObjectPtr<class UQEWB_WindowHandle> OwningHandle = nullptr;

	virtual void NativeDestruct() override;
};
