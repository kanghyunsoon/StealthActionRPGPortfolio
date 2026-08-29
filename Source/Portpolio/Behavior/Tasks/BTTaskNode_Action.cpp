#include "Behavior/Tasks/BTTaskNode_Action.h"
#include "Global.h"
#include "Characters/CEnemy_AI.h"
#include "Behavior/EnemyAIController.h"
#include "Components/CStateComponent.h"
#include "Components/CWeaponComponent.h"
#include "Weapons/DoActions/CDoAction.h"
#include "Weapons/CWeaponstructures.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTaskNode_Action::UBTTaskNode_Action()
{
	NodeName = "Action";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTaskNode_Action::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);


	AEnemyAIController* controller = Cast<AEnemyAIController>(OwnerComp.GetOwner());
	ACEnemy_AI* ai = Cast<ACEnemy_AI>(controller->GetPawn());


	UCWeaponComponent* weapon = CHelpers::GetComponent<UCWeaponComponent>(ai);
	CheckNullResult(weapon, EBTNodeResult::Failed);

	controller->StopMovement();
	weapon->DoAction();

	if (weapon->GetDoAction()->EnemyIndex < 4)
		weapon->GetDoAction()->EnemyIndex++;

	return EBTNodeResult::InProgress;
}

void UBTTaskNode_Action::TickTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);


	AEnemyAIController* controller = Cast<AEnemyAIController>(OwnerComp.GetOwner());
	ACEnemy_AI* ai = Cast<ACEnemy_AI>(controller->GetPawn());


	UCWeaponComponent* weapon = CHelpers::GetComponent<UCWeaponComponent>(ai);
	UCStateComponent*   state = CHelpers::GetComponent<UCStateComponent>(ai);

	bool bCheck = true;
	bCheck &= (state->IsIdleMode());
	bCheck &= (weapon->GetDoAction()->GetInAction() == false);

	// 여기부터 볼 것
	if (weapon->GetDoAction()->EnemyIndex >= 3)
		weapon->GetDoAction()->EnemyIndex = 0;


	if (bCheck)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

}

EBTNodeResult::Type UBTTaskNode_Action::AbortTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	Super::AbortTask(OwnerComp, NodeMemory);


	AEnemyAIController* controller = Cast<AEnemyAIController>(OwnerComp.GetOwner());
	ACEnemy_AI* ai = Cast<ACEnemy_AI>(controller->GetPawn());



	if (!ai) return EBTNodeResult::Failed;

	UCWeaponComponent* weapon = CHelpers::GetComponent<UCWeaponComponent>(ai);
	UCStateComponent*   state = CHelpers::GetComponent<UCStateComponent>(ai);

	if (weapon == nullptr)
		return EBTNodeResult::Failed;

	bool bBeginAction = weapon->GetDoAction()->GetBeginAction();
	if (!bBeginAction)
		weapon->GetDoAction()->Begin_DoAction();
	weapon->GetDoAction()->End_DoAction();

	weapon->GetDoAction()->EnemyIndex =0;
	return EBTNodeResult::Succeeded;
}
