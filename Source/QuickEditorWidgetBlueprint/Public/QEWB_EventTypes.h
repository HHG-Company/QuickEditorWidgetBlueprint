#pragma once

#include "CoreMinimal.h"
#include "QEWB_EventTypes.generated.h"

UENUM(BlueprintType)
enum class EQEWB_EventType : uint8
{
    Clicked,
    ValueChanged,
    SelectionChanged,
    FoldoutChanged,
    Closed,
    Confirmed,
    Cancelled
};

UENUM(BlueprintType)
enum class EQEWB_ValueType : uint8
{
    None,
    Bool,
    Int,
    Float,
    String,
    Name,
    Object,
    Class,
    Vector
};

UENUM(BlueprintType)
enum class EQEWB_SlotRule : uint8
{
    Fill UMETA(DisplayName="Fill"),
    Auto UMETA(DisplayName="Auto")
};

USTRUCT(BlueprintType)
struct FQEWB_Event
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="QuickEditorWidgetBlueprint")
    EQEWB_EventType Type = EQEWB_EventType::Clicked;

    UPROPERTY(BlueprintReadOnly, Category="QuickEditorWidgetBlueprint")
    FName Id = NAME_None;

    UPROPERTY(BlueprintReadOnly, Category="QuickEditorWidgetBlueprint")
    EQEWB_ValueType ValueType = EQEWB_ValueType::None;

    UPROPERTY(BlueprintReadOnly, Category="QuickEditorWidgetBlueprint")
    bool BoolValue = false;

    UPROPERTY(BlueprintReadOnly, Category="QuickEditorWidgetBlueprint")
    int32 IntValue = 0;

    UPROPERTY(BlueprintReadOnly, Category="QuickEditorWidgetBlueprint")
    float FloatValue = 0.f;

    UPROPERTY(BlueprintReadOnly, Category="QuickEditorWidgetBlueprint")
    FString StringValue;

    UPROPERTY(BlueprintReadOnly, Category="QuickEditorWidgetBlueprint")
    FName NameValue = NAME_None;

    UPROPERTY(BlueprintReadOnly, Category="QuickEditorWidgetBlueprint")
    TObjectPtr<UObject> ObjectValue = nullptr;

    UPROPERTY(BlueprintReadOnly, Category="QuickEditorWidgetBlueprint")
    TObjectPtr<UClass> ClassValue = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "QuickEditorWidgetBlueprint")
    FVector VectorValue = FVector::ZeroVector;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQEWB_OnEvent, const FQEWB_Event&, Event);
