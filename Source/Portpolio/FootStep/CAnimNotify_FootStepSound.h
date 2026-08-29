#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CAnimNotify_FootStepSound.generated.h"

UCLASS(const, hidecategories = Object, collapsecategories, meta = (DisplayName = "Play PMBFootStep Sound"))
class PORTPOLIO_API UCAnimNotify_FootStepSound : public UAnimNotify
{
	GENERATED_BODY()

public:
	FString GetNotifyName_Implementation() const override;//Notify 이름을 지어주는 함수.

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (DisplayName = "PMB Sound List"))
		TSubclassOf<class URes_PMBFootStepSoundList> m_pRef_SoundList;

	UPROPERTY()
		class URes_PMBFootStepSoundList* m_pSoundList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
		float m_fVolumeMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
		float m_fPitchMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
		uint32 m_bSoundAttach : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
		FVector m_vLocationOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
		float m_fCheckDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
		FName SocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
		uint32 m_bShowDebugLine : 1;

public:
	UCAnimNotify_FootStepSound();
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

public:
	void FootStep(USkeletalMeshComponent* MeshComp);

};
