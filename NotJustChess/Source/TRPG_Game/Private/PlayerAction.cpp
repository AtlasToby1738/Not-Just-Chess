#include "PlayerAction.h"
#include "GridManager.h"

void UPlayerAction::ExecuteAction(int Index)
{
}

void UPlayerAction::LeftClick(int Index)
{
	UE_LOG(LogTemp, Display, TEXT("The LeftClick has been executed"));
}

void UPlayerAction::RightClick(int Index)
{
	UE_LOG(LogTemp, Display, TEXT("The RightClick has been executed"));
}
