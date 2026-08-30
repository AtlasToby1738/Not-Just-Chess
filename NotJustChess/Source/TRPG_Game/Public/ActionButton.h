#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Parent_Pawn.h"
#include "ActionButton.generated.h"

class UPlayerAction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAdditionalAction);

UCLASS()
class TRPG_GAME_API UActionButton : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "PlayerAction")
	class TSubclassOf<UPlayerAction> ActionClass;

	UPROPERTY(EditAnywhere, Category = "PlayerAction")
	AParent_Pawn* TargetPawn;

	UPROPERTY(BlueprintAssignable, Category = "PlayerAction")
	FOnAdditionalAction OnAdditionalAction;

protected:
	virtual void NativeConstruct() override;
	UPlayerAction* myAction;
	class AGridManager* myGrid;

	UPROPERTY(EditAnywhere, meta = (BindWidget), Category = "Widget")
	class UButton* ActionButton;

private:

	UFUNCTION(BlueprintCallable)
	void ApplyAction();
	
};
