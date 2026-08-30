#include "HoveredAction.h"
#include "GridManager.h"

void UHoveredAction::ExecuteAction(int Index)
{
	if (GridRef->isIndexValid(Index))
	{
		if (GridRef->isIndexValid(GridRef->CurrentIndex))
		{
			UnHoverTile(GridRef->CurrentIndex);
			GridRef->AddStateToTile(ETileState::Hovered, GridRef->CurrentIndex);
		}
	}

	else
	{
		UnHoverTile(GridRef->PreviousIndex);
	}
}

void UHoveredAction::UnHoverTile(int index)
{
	GridRef->RemoveStateToTile(ETileState::Hovered, GridRef->PreviousIndex);
}