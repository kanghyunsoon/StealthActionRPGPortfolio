#include "Behavior/Services/BTService_Range.h"
#include "Global.h"
#include "GameFramework/Character.h"
#include "Behavior/EnemyAIController.h"
#include "Characters/CEnemy_AI.h"
#include "Components/CStateComponent.h"
#include "Behavior/CAIBehaviorComponent.h"

UBTService_Range::UBTService_Range()
{
	NodeName = "Range";

	Interval = 0.1f;
	RandomDeviation = 0.0f;
}

void UBTService_Range::TickNode(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AEnemyAIController* controller = Cast<AEnemyAIController>(OwnerComp.GetOwner());
	ACEnemy_AI* ai = Cast<ACEnemy_AI>(controller->GetPawn());
	UCStateComponent*       state = CHelpers::GetComponent<UCStateComponent>(ai);
	UCAIBehaviorComponent*  behavior = CHelpers::GetComponent<UCAIBehaviorComponent>(ai);

	if (bDrawDebug)
	{
		FVector start = ai->GetActorLocation();
		start.Z -= 25;


		FVector end = ai->GetActorLocation();
		end.Z += 25;

		DrawDebugCylinder(ai->GetWorld(), start, end, AvoidRange, 10, FColor::Red, false, Interval);
	}


	if (state->IsHitMode())
	{
		behavior->SetHittedMode();
		return;
	}

	ACharacter* target = behavior->GetTarget();
	if (target == nullptr)
	{
		controller->ClearFocus(EAIFocusPriority::Gameplay);
		behavior->SetWaitMode();
		return;
	}

	controller->SetFocus(target);



	float distance = ai->GetDistanceTo(target);
	if (distance < AvoidRange)
	{
		behavior->SetAvoidMode();
		return;
	}
	behavior->SetActionMode();

}
