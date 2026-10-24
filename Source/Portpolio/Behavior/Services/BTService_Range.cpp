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

	// 적이 죽어 폰이 제거되거나 빙의가 풀린 프레임에도 서비스가 한 번 더 돌 수 있다.
	AEnemyAIController* controller = Cast<AEnemyAIController>(OwnerComp.GetOwner());
	if (controller == nullptr)
		return;
	ACEnemy_AI* ai = Cast<ACEnemy_AI>(controller->GetPawn());
	if (ai == nullptr)
		return;
	UCStateComponent*       state = CHelpers::GetComponent<UCStateComponent>(ai);
	UCAIBehaviorComponent*  behavior = CHelpers::GetComponent<UCAIBehaviorComponent>(ai);
	if (state == nullptr || behavior == nullptr)
		return;

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
