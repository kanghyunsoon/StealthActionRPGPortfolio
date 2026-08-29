#include "FootStep/Res_PMBFootStepSoundList.h"
#include "Sound/SoundBase.h"

USoundBase* URes_PMBFootStepSoundList::GetSound(FString sSoundName)
{
	return m_pSoundList.FindRef(sSoundName);
}
