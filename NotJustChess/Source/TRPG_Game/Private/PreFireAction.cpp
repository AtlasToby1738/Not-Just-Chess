#include "PreFireAction.h"
#include "GridManager.h"

void UPreFireAction::ExecuteAction(int Index)
{
	if (GridRef->isIndexValid(Index))
	{
		if (GridRef->currentPawnType == EPawnType::King) return;
		if (GridRef->isIndexValid(GridRef->CurrentIndex))
		{
			if (!GridRef->TileInRangeForAttack.Contains(GridRef->CurrentIndex))
			{
				if (GridRef->currentPawnType == EPawnType::Queen)
				{
					for (int _tile : GridRef->TileInRangeForAttack)
					{
						UnHoverTile(_tile);
					}
					return;
				}
				UnHoverTile(GridRef->PreviousIndex); return;
			}
			switch (GridRef->currentPawnType)
			{
			case EPawnType::Pawn:
			{
				UnHoverTile(GridRef->PreviousIndex);
				GridRef->AddStateToTile(ETileState::PreFire, GridRef->CurrentIndex);
				break;
			}
			case EPawnType::Knight:
			{
				UnHoverTile(GridRef->PreviousIndex);
				GridRef->AddStateToTile(ETileState::PreFire, GridRef->CurrentIndex);
				break;
			}
			case EPawnType::Tower:
			{
				UnHoverTile(GridRef->PreviousIndex);
				GridRef->AddStateToTile(ETileState::PreFire, GridRef->CurrentIndex);
				break;
			}
			case EPawnType::Bishop:
			{
				UnHoverTile(GridRef->PreviousIndex);
				GridRef->AddStateToTile(ETileState::PreFire, GridRef->CurrentIndex);
				break;
			}
			case EPawnType::Queen:
			{
				for (int _tile : GridRef->TileInRangeForAttack)
				{
					UnHoverTile(_tile);
				}
				GridRef->AddStateToTile(ETileState::PreFire, GridRef->CurrentIndex);
				if (GridRef->TileInRangeForAttack.Contains(GridRef->CurrentIndex - 1))
					GridRef->AddStateToTile(ETileState::PreFire, GridRef->CurrentIndex - 1);
				if (GridRef->TileInRangeForAttack.Contains(GridRef->CurrentIndex + 1))
					GridRef->AddStateToTile(ETileState::PreFire, GridRef->CurrentIndex + 1);
				if (GridRef->TileInRangeForAttack.Contains(GridRef->CurrentIndex + GridRef->Y))
					GridRef->AddStateToTile(ETileState::PreFire, GridRef->CurrentIndex + GridRef->Y);
				if (GridRef->TileInRangeForAttack.Contains(GridRef->CurrentIndex - GridRef->Y))
					GridRef->AddStateToTile(ETileState::PreFire, GridRef->CurrentIndex - GridRef->Y);
				break;
			}
			}
		}
	}
	else
	{
		UnHoverTile(GridRef->PreviousIndex);
	}
}

void UPreFireAction::UnHoverTile(int index)
{
	GridRef->RemoveStateToTile(ETileState::PreFire, index);
}
