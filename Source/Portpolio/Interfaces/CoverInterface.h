#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CoverInterface.generated.h"

class UAnimMontage;

UINTERFACE(MinimalAPI)
class UCoverInterface : public UInterface
{
	GENERATED_BODY()
};

class PORTPOLIO_API ICoverInterface
{
	GENERATED_BODY()

public:
	virtual void GetCoverStatus(bool& InCover, bool& InCoverMovRight, bool& InCoverMovLeft, bool& InCoverIdleRight) {};
	virtual void CoverUpdate(bool bCoverUpdate) {};
	virtual void CoverToCoverLeft(bool bCoverToCoverLeft) {};
	virtual void CoverToCoverRight(bool bCoverToCoverRight) {};
	virtual void CoverGetIsCrouched(bool& OutbCrouched) {};
	virtual void CoverCrouchUpdate(bool& bCrouched) {};
	virtual void GetCoverMontages(UAnimMontage*& In, UAnimMontage*& Out, UAnimMontage*& InCrouched, UAnimMontage*& OutCrouched) {};
	virtual void CoverExit() {};
	virtual void SpeedUpdate(bool bSprinting) {};
	virtual void StealthKill() {};
};
