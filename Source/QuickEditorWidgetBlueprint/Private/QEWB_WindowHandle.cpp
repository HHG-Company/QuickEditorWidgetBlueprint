#include "QEWB_WindowHandle.h"

void UQEWB_WindowHandle::Cleanup()
{
	if (bIsCleanedUp)
	{
		return;
	}

	bIsCleanedUp = true;

	if (ModalWindow.IsValid())
	{
		ModalWindow.Reset();
	}

	NonModalHost = nullptr;
	NonModalWindow = nullptr;

	ModalHost = nullptr;
	bModalShown = false;

	LayoutStack.Reset();
	Events.Reset();
	WidgetsById.Reset();

	BoolValues.Reset();
	IntValues.Reset();
	FloatValues.Reset();
	StringValues.Reset();
	NameValues.Reset();
	ObjectValues.Reset();
	ClassValues.Reset();

	PickerModelsById.Reset();
	PickerProxiesById.Reset();

	OnButtonClickedById.Reset();
	OnBoolChangedById.Reset();
	OnIntChangedById.Reset();
	OnFloatChangedById.Reset();
	OnStringChangedById.Reset();
	OnNameChangedById.Reset();
	OnObjectChangedById.Reset();
	OnClassChangedById.Reset();

	OwnedUObjects.Reset();
}