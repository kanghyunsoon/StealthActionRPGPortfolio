#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_Melee.generated.h"

UCLASS()
class PORTPOLIO_API UBTService_Melee : public UBTService
{
	GENERATED_BODY()


private:
	UPROPERTY(EditAnywhere, Category = "Action")
		float  ActionRange = 150;

public:
	UBTService_Melee();

protected:
	void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
