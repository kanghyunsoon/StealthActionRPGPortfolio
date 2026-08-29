#include "Behavior/CAIBehaviorComponent.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"



UCAIBehaviorComponent::UCAIBehaviorComponent()
{


}

void UCAIBehaviorComponent::BeginPlay()
{
	Super::BeginPlay();



}

FVector UCAIBehaviorComponent::GetPatrolLocation()
{
	return Blackboard->GetValueAsVector(PatrolLocationKey);
}

void UCAIBehaviorComponent::SetPatrolLocation(const FVector & InLocation)
{
	Blackboard->SetValueAsVector(PatrolLocationKey, InLocation);
}

FVector UCAIBehaviorComponent::GetAvoidLocation()
{
	return Blackboard->GetValueAsVector(AvoidLocationKey);
}

EAIStateType UCAIBehaviorComponent::GetType()
{
	return (EAIStateType)Blackboard->GetValueAsEnum(AIStateTypeKey);
}

void UCAIBehaviorComponent::ChangeType(EAIStateType InType)
{
	EAIStateType preType = GetType();

	Blackboard->SetValueAsEnum(AIStateTypeKey, (uint8)InType);

	if (OnAIStateTypeChanged.IsBound())
		OnAIStateTypeChanged.Broadcast(preType, InType);

}

ACharacter * UCAIBehaviorComponent::GetTarget()
{
	return Cast<ACharacter>(Blackboard->GetValueAsObject(TargetKey));
}

bool UCAIBehaviorComponent::IsWaitMode()
{
	return GetType() == EAIStateType::Wait;
}

bool UCAIBehaviorComponent::IsApproachMode()
{
	return GetType() == EAIStateType::Approach;
}

bool UCAIBehaviorComponent::IsActionMode()
{
	return GetType() == EAIStateType::Action;
}

bool UCAIBehaviorComponent::IsPatrolMode()
{
	return GetType() == EAIStateType::Patrol;
}

bool UCAIBehaviorComponent::IsHittedMode()
{
	return GetType() == EAIStateType::Hitted;
}

bool UCAIBehaviorComponent::IsAvoidMode()
{
	return GetType() == EAIStateType::Avoid;
}

bool UCAIBehaviorComponent::IsDeadMode()
{
	return GetType() == EAIStateType::Dead;
}

void UCAIBehaviorComponent::SetWaitMode()
{
	ChangeType(EAIStateType::Wait);
}
////////////////////////////////////////////////////////////////////////////////
void UCAIBehaviorComponent::SetApproachMode()
{
	bCanTakeDown = false;
	ChangeType(EAIStateType::Approach);
}

void UCAIBehaviorComponent::SetActionMode()
{
	bCanTakeDown = false;
	ChangeType(EAIStateType::Action);
}

void UCAIBehaviorComponent::SetPatrolMode()
{
	bCanTakeDown = true;
	ChangeType(EAIStateType::Patrol);
}

void UCAIBehaviorComponent::SetHittedMode()
{
	bCanTakeDown = false;
	ChangeType(EAIStateType::Hitted);
}

void UCAIBehaviorComponent::SetAvoidMode()
{
	bCanTakeDown = false;
	ChangeType(EAIStateType::Avoid);
}

void UCAIBehaviorComponent::SetDeadMode()

{
	bCanTakeDown = false;
	ChangeType(EAIStateType::Dead);
}
