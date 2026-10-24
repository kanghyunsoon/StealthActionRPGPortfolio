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
	ACEnemy_AI* ai = controller ? Cast<ACEnemy_AI>(controller->GetPawn()) : nullptr;
	if (ai == nullptr)
		return EBTNodeResult::Failed;


	UCWeaponComponent* weapon = CHelpers::GetComponent<UCWeaponComponent>(ai);
	CheckNullResult(weapon, EBTNodeResult::Failed);
	// 무기를 들지 않은 상태면 DoAction이 없다.
	if (weapon->GetDoAction() == nullptr)
		return EBTNodeResult::Failed;

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
	ACEnemy_AI* ai = controller ? Cast<ACEnemy_AI>(controller->GetPawn()) : nullptr;
	if (ai == nullptr)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}


	UCWeaponComponent* weapon = CHelpers::GetComponent<UCWeaponComponent>(ai);
	UCStateComponent*   state = CHelpers::GetComponent<UCStateComponent>(ai);
	if (weapon == nullptr || state == nullptr || weapon->GetDoAction() == nullptr)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

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
	ACEnemy_AI* ai = controller ? Cast<ACEnemy_AI>(controller->GetPawn()) : nullptr;

	// AbortTask는 Aborted(또는 InProgress)를 돌려줘야 한다. 예전에는 Succeeded를 돌려줘서
	// 중단된 공격이 성공한 것처럼 처리될 수 있었다.
	if (!ai) return EBTNodeResult::Aborted;

	UCWeaponComponent* weapon = CHelpers::GetComponent<UCWeaponComponent>(ai);

	if (weapon == nullptr || weapon->GetDoAction() == nullptr)
		return EBTNodeResult::Aborted;

	bool bBeginAction = weapon->GetDoAction()->GetBeginAction();
	if (!bBeginAction)
		weapon->GetDoAction()->Begin_DoAction();
	weapon->GetDoAction()->End_DoAction();

	weapon->GetDoAction()->EnemyIndex =0;
	return EBTNodeResult::Aborted;
}
