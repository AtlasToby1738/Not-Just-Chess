#include "ActionButton.h"
#include <Components/Button.h>
#include "PlayerAction.h"
#include "GridManager.h"
#include "MovePawnAction.h"
#include "Kismet/GameplayStatics.h"


void UActionButton::NativeConstruct()
{
	UE_LOG(LogTemp, Warning, TEXT("CONSTRUCTOR ACTIVATED !!!!"));

	if (ActionButton && !ActionButton->OnClicked.IsBound())
	{
		ActionButton->OnClicked.AddDynamic(this, &UActionButton::ApplyAction);
	}
	if (ActionClass)
	{
		myAction = NewObject<UPlayerAction>(this, ActionClass);
	}
	myGrid = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
	if (myAction && myGrid)
	{
		myAction->Initialize(myGrid);

		UMovePawnAction* moveAction = Cast<UMovePawnAction>(myAction);
		if (moveAction && TargetPawn)
		{
			moveAction->Pawn = TargetPawn;
		}
	}
}

void UActionButton::ApplyAction()
{
	myGrid->CurrentAction = myAction;
	OnAdditionalAction.Broadcast();
}

