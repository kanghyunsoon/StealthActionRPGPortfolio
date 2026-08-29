#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/CoverInterface.h"
#include "Interfaces/ComponentsComunication.h"
#include "CCoverSystemComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PORTPOLIO_API UCCoverSystemComponent :
	public UActorComponent,
	public ICoverInterface,
	public IComponentsComunication
{
	GENERATED_BODY()



public:
	UPROPERTY(EditAnywhere, Category = "Trace") // DebugLine
		bool ShowTracers;

	UPROPERTY(EditDefaultsOnly, Category = "Trace") // DebugLine
	TEnumAsByte<EDrawDebugTrace::Type> TraceDebug;

	UPROPERTY(EditAnywhere, Category = "Montages") // DebugLine
		class UAnimMontage* GetInCoverMontage;

	UPROPERTY(EditAnywhere, Category = "Montages") // DebugLine
		class UAnimMontage* GetOutCoverMontage;

	UPROPERTY(EditAnywhere, Category = "Montages") // DebugLine
		class UAnimMontage* CrouchGetInCoverMontage;

	UPROPERTY(EditAnywhere, Category = "Montages") // DebugLine
		class UAnimMontage* CrouchGetOutCoverMontage;

	UPROPERTY(EditAnywhere, Category = "Montages") // DebugLine
		class UAnimMontage* CoverToCoverLeftMontage;

	UPROPERTY(EditAnywhere, Category = "Montages") // DebugLine
		class UAnimMontage* CoverToCoverRightMontage;

	UPROPERTY(EditAnywhere, Category = "Montages") // DebugLine
		class UAnimMontage* CrouchCoverToCoverLeftMontage;

	UPROPERTY(EditAnywhere, Category = "Montages") // DebugLine
		class UAnimMontage* CrouchCoverToCoverRightMontage;

	UCCoverSystemComponent();

	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SideTracerRightTimerFunc();
	void SideTracerLeftTimerFunc();

	// 좌우 탐지 기준점
	void SimulateRightArrow(FVector& OutLocation, FVector& OutForward, FVector& OutRight, FVector& OutUp);
	void SimulateLeftArrow(FVector& OutLocation, FVector& OutForward, FVector& OutRight, FVector& OutUp);

	// 커버 상태 이동
	void MoveInCover();
	void MoveRight();
	void MoveLeft();

	// 전방 커버 탐지
	void ForwardTracer();
	void CalculateTracePoints(FVector& OutStart, FVector& OutEnd, float RightVectorMultiplier);

	// 이동 속도
	virtual void SpeedUpdate(bool bSprinting) override;

	// 커버 높이와 전환 거리 탐지
	void CoverHeightTrace(FVector& OutStart, FVector& OutEnd);
	void CoverJumpRightTraceLocation(FVector& OutStart, FVector& OutEnd);
	void CoverJumpLeftTraceLocation(FVector& OutStart, FVector& OutEnd);
	void CoverJumpingRightTracer();
	void CoverJumpingLeftTracer();
	void InCoverHeightCheck();

	// 커버 위치 계산
	void CoverLocation(FVector& OutLocation);
	void CoverJumpLocation(FVector& OutLocation);

	// 좌우 이동 탐지
	void SideTracers();

	// 커버 간 이동
	void CoverToCoverJump(bool bInRight);

	// 커버 진입·종료
	void TakeCover();
	void ExitCover();
	void InCoverExitToGrab();

	// 커버 끝 카메라 보정
	void CameraEdgeZoom(bool bRight);
	void SetCameraMoverTimer(float Time);
	void CameraMove();

	// 입력 처리
	void ProcessRightMovement(float& bRightInputValue);
	void ProcessForwardMovement(float& bForwardInputValue);

	// 상태 동기화
	virtual void GetCoverMontages(UAnimMontage*& In, UAnimMontage*& Out, UAnimMontage*& InCrouched, UAnimMontage*& OutCrouched) override;
	virtual void CoverExit() override;
	virtual void StealthKill() override;
	virtual void InputBlocked(bool& Blocked) override;
	virtual void CoverGetIsCrouched(bool& OutbCrouched) override;
	virtual void CoverUpdate(bool bCoverUpdate) override;
	virtual void GetCoverStatus(bool& InCover, bool& InCoverMovRight, bool& InCoverMovLeft, bool& InCoverIdleRight) override;

	virtual void CSSync(FCoverSystem& CSStruct) override;
	virtual void TSSync(FTargetingSystem& TSStruct) override;
	virtual void GHSync(FGrappingHookSystem& GHStruct) override;
	virtual void NCSSync(FNCSSystem& NCSStruct) override;
	virtual void SLSSync(FLedgeSystem& SLSStruct) override;


	bool bInCoverMovRight;
	bool bInCoverMovLeft;
	bool bInCoverIdleRight;
	bool bRestoreCollisionTest;
	bool bUseInvertedCoverNormal;
	bool bCanCover;
	bool bInCover;
	bool bJumpingCoverToCover;
	bool bCanJumpToCoverLeft;
	bool bCanJumpToCoverRight;
	bool bAutomaticallyCoverJump;
	bool bRightInput;
	bool bCrouchAutomatically;
	bool bInCoverCrouched;
	bool bCameraZoomOnEdge;
	bool bCameraMoving;
	bool bCameraEdgeMove;
	bool bZoomRight;
	bool bIsInputBlocked;
	bool bRightDoOnceCompleted;
	bool bForwardDoOnceCompleted;
	bool bUseKeyToExitCover;

	FTimerHandle CameraMoveTimer;

	float RightInputValue;
	float InCoverTraceDistance = 60;
	float MaxWalkInCoverAngle = 90;
	float InCoverSpeedScalar = 1;
	float CoverToCoverMaxDistance = 160;
	float CoverRightInputValue;
	float CoverForwardInputValue;

	FVector CoverWallNormal;
	FVector CoverWallLocation;
	FVector NewCoverNormal;
	FVector NewCoverLocation;

	class USpringArmComponent* SpringArm;
	class ACPlayer* OwnerCharacter;
	class USkeletalMeshComponent* Mesh;
	class UCAnimInstance* AnimInstance;


	class UActorComponent* CS = nullptr;
	class UActorComponent* SLS = nullptr;
	class UActorComponent* GH = nullptr;
	class UActorComponent* NCS = nullptr;
	class UActorComponent* TS = nullptr;
	class UCameraComponent* AimCamera = nullptr;
	class USpringArmComponent* AimSpringArm = nullptr;
	class UCameraComponent*  MainCamera = nullptr;
	class UCableComponent* Cable = nullptr;
	class USphereComponent* Sphere = nullptr;
};
