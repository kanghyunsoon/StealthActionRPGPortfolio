#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Res_PMBFootStepSoundList.generated.h"

/**
 *
 */
UCLASS(Blueprintable)
class PORTPOLIO_API URes_PMBFootStepSoundList : public UObject
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound, Meta = (AllowPrivateAccess = true))
		TMap<FString, class USoundBase*> m_pSoundList;

public:
	class USoundBase* GetSound(FString sSoundName);
	int GetSoundListNum() { return m_pSoundList.Num(); }
};
