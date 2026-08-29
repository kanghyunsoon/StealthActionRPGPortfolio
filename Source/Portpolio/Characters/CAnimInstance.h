#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Components/CWeaponComponent.h"
#include "Components/CFeetComponent.h"
#include "Interfaces/CoverInterface.h"
#include "CAnimInstance.generated.h"

UCLASS()
class PORTPOLIO_API UCAnimInstance : public UAnimInstance, public ICoverInterface
{
	GENERATED_BODY()

public:
	void NativeBeginPlay() override;
	void NativeUpdateAnimation(float DeltaSeconds) override;



	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
		bool bDashed;


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
		float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
		float Pitch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
		float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
		bool bFalling;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
		EWeaponType WeaponType = EWeaponType::Max;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InverseKinemetics")
		bool bFeet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InverseKinemetics")
		FFeetData FeetData;


private:
	UFUNCTION()
	void OnWeaponTypeChanged(EWeaponType InPrevType, EWeaponType InNewType);

private:
	class ACharacter* OwnerCharacter;
	class ACPlayer* Player;
	class UCWeaponComponent* Weapon;


private:
	FRotator PrevRotation;



};
