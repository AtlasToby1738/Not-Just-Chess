#include "MovePawnAction.h"
#include "GridManager.h"
#include "Parent_Pawn.h"


void UMovePawnAction::LeftClick(int Index)
{
	if (!GridRef || !Pawn) { UE_LOG(LogTemp, Display, TEXT("Failled to find grid or pawn")); return; }

    for (int tile : Pawn->MovementTiles)
        GridRef->AddStateToTile(ETileState::Accessible, tile);
}
