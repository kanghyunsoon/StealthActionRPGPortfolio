#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ComponentsComunication.generated.h"

USTRUCT()
struct FLedgeSystem
{
	GENERATED_BODY()

public:
	bool bIsHanging;
	bool bIsClimbingEdge;
	bool bCornerMoving;
};

USTRUCT()
struct FCoverSystem
{
	GENERATED_BODY()

public:
	bool bInCover;
	bool bCanMoveRight;
	bool bCanMoveLeft;
	bool bCanCover;
};

USTRUCT()
struct FGrappingHookSystem
{
	GENERATED_BODY()

public:
	bool bLaunchingCable;
	bool bAiming;
	float MaxDistance;
};

USTRUCT()
struct FNCSSystem
{
	GENERATED_BODY()

public:
	float Health;
	bool bAiming;
	bool bCombatMode;
	bool bRolling;
	int32 MovementMode;
	bool bGrabbingBody;
	bool bGrabbingEnemy;
	bool bCanStealthKill;
	bool bEnemyIsAttacking;
	bool bEnemyIsBlocking;
	bool bWeaponEquipped;
	bool bDead;
	float ProjectileMaxDistance;
};


USTRUCT()
struct FTargetingSystem
{
	GENERATED_BODY()

public:
	bool bLockOnEnabled;
	class AActor* TargetedEnemy;
	TArray<AActor*> AllEnemies;
};

UINTERFACE(MinimalAPI)
class UComponentsComunication : public UInterface
{
	GENERATED_BODY()
};

class PORTPOLIO_API IComponentsComunication
{
	GENERATED_BODY()

public:
	virtual void SLSSync(FLedgeSystem& SLSStruct) {};
	virtual void CSSync(FCoverSystem& CSStruct) {};
	virtual void GHSync(FGrappingHookSystem& GHStruct) {};
	virtual void NCSSync(FNCSSystem& NCSStruct) {};
	virtual void TSSync(FTargetingSystem& TSStruct) {};
	virtual void InputBlocked(bool& Blocked) {};
};
