#include "UnSelectAction.h"
#include "GridManager.h"

void UUnSelectAction::RightClick(int Index)
{
	GridRef->RemoveStateToTile(ETileState::Selected, GridRef->CurrentIndex);
}
