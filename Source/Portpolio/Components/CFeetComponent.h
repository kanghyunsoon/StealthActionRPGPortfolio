#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CFeetComponent.generated.h"

USTRUCT(BlueprintType)
struct FFeetData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Feet")
		FVector LeftDistance;	// X축

	UPROPERTY(BlueprintReadOnly, Category = "Feet")
		FVector RightDistance;	// X축

	UPROPERTY(BlueprintReadOnly, Category = "Feet")
		FVector PelvisDistance;	// Z축

	UPROPERTY(BlueprintReadOnly, Category = "Feet")
		FRotator LeftRotation;

	UPROPERTY(BlueprintReadOnly, Category = "Feet")
		FRotator RightRotation;

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PORTPOLIO_API UCFeetComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Trace")
		TEnumAsByte<EDrawDebugTrace::Type> DrawDebug;

	UPROPERTY(EditAnywhere, Category = "Trace")
		float InterpSpeed = 50.0f;

	UPROPERTY(EditAnywhere, Category = "Trace")
		float TraceDistance = 50.0f;

	UPROPERTY(EditAnywhere, Category = "Trace")
		float OffsetDistance = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Trace")
		FName LeftSocketName = "Foot_L";

	UPROPERTY(EditAnywhere, Category = "Trace")
		FName RightSocketName = "Foot_R";

public:
	FORCEINLINE FFeetData GetData() { return Data; }

public:
	UCFeetComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void Trace(FName InName, float& OutDistance, FRotator& OutRotation);

private:
	class ACharacter* OwnerCharacter;
	FFeetData Data;

};
