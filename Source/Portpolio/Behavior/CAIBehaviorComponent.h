
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CAIBehaviorComponent.generated.h"

UENUM(BlueprintType)
enum class EAIStateType : uint8
{
	Wait = 0, Approach, Action, Patrol, Hitted, Avoid, Dead, Max
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAIStateTypeChanged, EAIStateType, InPrevType, EAIStateType, InNewType);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PORTPOLIO_API UCAIBehaviorComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Key")
		FName AIStateTypeKey = "AIState";

	UPROPERTY(EditAnywhere, Category = "Key")
		FName TargetKey = "Target";

	UPROPERTY(EditAnywhere, Category = "Key")
		FName PatrolLocationKey = "Patrol_Location";


	UPROPERTY(EditAnywhere, Category = "Key")
		FName AvoidLocationKey = "Avoid_Location";

private:
	EAIStateType GetType();
	void         ChangeType(EAIStateType InType);

public:
	class  ACharacter* GetTarget();


public:
	bool IsWaitMode();
	bool IsApproachMode();
	bool IsActionMode();
	bool IsPatrolMode();
	bool IsHittedMode();
	bool IsAvoidMode();
	bool IsDeadMode();

public:
	void SetWaitMode();
	void SetApproachMode();
	void SetActionMode();
	void SetPatrolMode();
	void SetHittedMode();
	void SetAvoidMode();
	void SetDeadMode();


public:
	UCAIBehaviorComponent();

protected:
	virtual void BeginPlay() override;

public:
	FORCEINLINE void SetBlackboard(class UBlackboardComponent* InBlackboard) { Blackboard = InBlackboard; }
	FORCEINLINE bool GetTakeDown() { return bCanTakeDown; }

public:  //패트롤
	FVector GetPatrolLocation();
	void    SetPatrolLocation(const FVector& InLocation);

public:  //Avoid
	FVector GetAvoidLocation();



public:
	FAIStateTypeChanged  OnAIStateTypeChanged;

private:
	class  UBlackboardComponent* Blackboard;
	bool bCanTakeDown = true;





};
