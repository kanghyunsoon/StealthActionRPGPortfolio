
#include "Behavior/Services/BTService_Melee.h"
#include "Global.h"
#include "GameFramework/Character.h"
#include "Behavior/EnemyAIController.h"
#include "Characters/CEnemy_AI.h"
#include "Components/CStateComponent.h"
#include "Behavior/CAIBehaviorComponent.h"


UBTService_Melee::UBTService_Melee()
{
	NodeName = "Melee";

	Interval = 0.1f;
	RandomDeviation = 0.0f;
}

void UBTService_Melee::TickNode(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AEnemyAIController* controller = Cast<AEnemyAIController>(OwnerComp.GetOwner());
	ACEnemy_AI* ai = Cast<ACEnemy_AI>(controller->GetPawn());


	//
	UCStateComponent*       state = CHelpers::GetComponent<UCStateComponent>(ai);
	UCAIBehaviorComponent*  behavior = CHelpers::GetComponent<UCAIBehaviorComponent>(ai);


	if (state->IsHitMode())
	{
		behavior->SetHittedMode();
		return;
	}

	ACharacter* target = behavior->GetTarget();
	if (target == nullptr)
	{
		behavior->SetPatrolMode();
		return;
	}

	float distance = ai->GetDistanceTo(target);
	if (distance < ActionRange)
	{
		behavior->SetActionMode();
		return;
	}
	behavior->SetApproachMode();
}
