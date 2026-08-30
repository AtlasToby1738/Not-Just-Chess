#include "SelectedAction.h"
#include "GridManager.h"

void USelectedAction::LeftClick(int Index)
{
	GridRef->AddStateToTile(ETileState::Selected, GridRef->CurrentIndex);
}

