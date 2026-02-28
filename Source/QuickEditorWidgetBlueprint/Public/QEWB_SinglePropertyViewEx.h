#pragma once

#include "CoreMinimal.h"
#include "Components/SinglePropertyView.h"
#include "QEWB_SinglePropertyViewEx.generated.h"

UCLASS()
class QUICKEDITORWIDGETBLUEPRINT_API UQEWB_SinglePropertyViewEx : public USinglePropertyView
{
    GENERATED_BODY()

public:
    auto& GetOnPropertyChangedPublic()
    {
        return OnPropertyChanged;
    }
};
