#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_Range.generated.h"


UCLASS()
class PORTPOLIO_API UBTService_Range : public UBTService
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Action")
		float AvoidRange = 500;
	UPROPERTY(EditAnywhere, Category = "Action")
		bool bDrawDebug;

public:
	UBTService_Range();

protected:
	void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
