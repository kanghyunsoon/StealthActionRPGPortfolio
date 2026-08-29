#include "Components/CCoverSystemComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Global.h"
#include "GameFramework/SpringArmComponent.h"
#include "Characters/CPlayer.h"
#include "Characters/CAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"

UCCoverSystemComponent::UCCoverSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UCCoverSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACPlayer>(GetOwner());
	CheckNull(OwnerCharacter);

	Mesh = OwnerCharacter->GetMesh();

	AnimInstance = Cast<UCAnimInstance>(Mesh->GetAnimInstance());

	OwnerCharacter->AllComponent(CS, SLS, GH, NCS, TS, AimCamera, AimSpringArm, MainCamera, SpringArm, Cable, Sphere);

	if (ShowTracers)
	{
		TraceDebug = EDrawDebugTrace::ForOneFrame;
	}
}


void UCCoverSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bInCover)
	{
		SideTracers();
		MoveInCover();

		if (!bInCoverMovRight && !bJumpingCoverToCover)
		{
			CoverJumpingRightTracer();
		}

		if (!bInCoverMovLeft && !bJumpingCoverToCover)
		{
			CoverJumpingLeftTracer();
		}

		if (bCrouchAutomatically && bInCoverMovRight && bInCoverMovLeft && bJumpingCoverToCover)
		{
			InCoverHeightCheck();
		}
	}
	else
	{
		ForwardTracer();
	}

}

// 좌우 탐지 기준점
void UCCoverSystemComponent::SimulateRightArrow(FVector & OutLocation, FVector & OutForward, FVector & OutRight, FVector & OutUp)
{

	FVector Forward = OwnerCharacter->GetActorForwardVector() * 42.0f;
	FVector Right = OwnerCharacter->GetActorRightVector() * 70.0f;
	FVector Up = OwnerCharacter->GetActorUpVector() * 40.0f;

	FRotator Rotation = UKismetMathLibrary::MakeRotationFromAxes(Forward, Right, Up);

	OutLocation = OwnerCharacter->GetActorLocation() + Forward + Right + Up;
	OutForward = UKismetMathLibrary::GetForwardVector(Rotation);
	OutRight = UKismetMathLibrary::GetRightVector(Rotation) * 1;
	OutUp = UKismetMathLibrary::GetRightVector(Rotation) * 1;
}

void UCCoverSystemComponent::SimulateLeftArrow(FVector & OutLocation, FVector & OutForward, FVector & OutRight, FVector & OutUp)
{
	FVector Forward = OwnerCharacter->GetActorForwardVector() * 42.0f;
	FVector Right = OwnerCharacter->GetActorRightVector() * -70.0f;
	FVector Up = OwnerCharacter->GetActorUpVector() * 40.0f;

	FRotator Rotation = UKismetMathLibrary::MakeRotationFromAxes(Forward, Right, Up);

	OutLocation = OwnerCharacter->GetActorLocation() + Forward + Right + Up;
	OutForward = UKismetMathLibrary::GetForwardVector(Rotation);
	OutRight = UKismetMathLibrary::GetRightVector(Rotation) * -1;
	OutUp = UKismetMathLibrary::GetRightVector(Rotation) * -1;
}

// 커버 상태 이동
void UCCoverSystemComponent::MoveInCover()
{
	if (RightInputValue == 0)
	{
		bInCoverMovRight = false;
		bInCoverMovLeft = false;
		SpringArm->bDoCollisionTest = bRestoreCollisionTest;
	}
	else
	{
		SpringArm->bDoCollisionTest = false;
	}

	MoveRight();
	MoveLeft();
}

void UCCoverSystemComponent::MoveRight()
{
	FVector Forward = UKismetMathLibrary::GetForwardVector(OwnerCharacter->GetActorRotation());
	FVector Right = UKismetMathLibrary::GetRightVector(OwnerCharacter->GetActorRotation());
	FVector Start = OwnerCharacter->GetActorLocation() + (Right * InCoverTraceDistance);
	FVector End = Start + (Forward * -60) + (Right * 20);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter); // 자신의 액터를 무시합니다.


	FHitResult HitResult;

	FVector WallRNormal;

	if (bInCoverMovRight && RightInputValue < 0)
	{
		// 스피어 트레이스를 수행합니다.
		bool bHit = GetWorld()->SweepSingleByChannel(HitResult, Start, End, FQuat::Identity, ECC_GameTraceChannel1,
			FCollisionShape::MakeSphere(20.0f), QueryParams);

		// 디버그 드로잉
		if (TraceDebug != EDrawDebugTrace::None)
		{
			DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f, 0, 1.0f);
		}

		if (bHit)
		{
			float DotProduct = FVector::DotProduct(HitResult.ImpactNormal, CoverWallNormal);
			float AngleInDegrees = FMath::RadiansToDegrees(acosf(DotProduct));
			int TruncatedValue = FMath::TruncToInt(AngleInDegrees);

			if (MaxWalkInCoverAngle + 1 < (float)TruncatedValue)
			{
				WallRNormal = HitResult.ImpactNormal;

				{
					FVector Target = (WallRNormal * 30) + HitResult.ImpactPoint;
					float InterpSpeed = (InCoverTraceDistance / 20) * InCoverSpeedScalar;
					FVector NewLocation = FMath::VInterpTo(OwnerCharacter->GetActorLocation(), Target, GetWorld()->GetDeltaSeconds(), InterpSpeed);
					OwnerCharacter->SetActorLocation(NewLocation);
				}

				float Multiplier = bUseInvertedCoverNormal ? -1.0f : 1.0f;

				FVector MultipliedVector = UKismetMathLibrary::Multiply_VectorFloat(WallRNormal, Multiplier);
				FRotator RotFromVector = UKismetMathLibrary::Conv_VectorToRotator(MultipliedVector);

				FRotator NewRotation = UKismetMathLibrary::RInterpTo(OwnerCharacter->GetActorRotation(), RotFromVector, GetWorld()->GetDeltaSeconds(), 5);
				bool bSuccess = OwnerCharacter->SetActorRotation(NewRotation);

				bInCoverMovRight = true;
				bInCoverMovLeft = false;
				bInCoverIdleRight = false;

				if (bSuccess)
					CoverWallNormal = WallRNormal;
			}
			else
			{
				bInCoverMovRight = false;
			}
		}
	}
	else
	{
		bInCoverMovRight = false;
	}
}

void UCCoverSystemComponent::MoveLeft()
{

	FVector Forward = UKismetMathLibrary::GetForwardVector(OwnerCharacter->GetActorRotation());
	FVector Right = UKismetMathLibrary::GetRightVector(OwnerCharacter->GetActorRotation());
	FVector Start = OwnerCharacter->GetActorLocation() + (Right * (InCoverTraceDistance * -1));
	FVector End = Start + (Forward * -60) + (Right * -20);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter); // 자신의 액터를 무시합니다.


	FHitResult HitResult;

	FVector WallLNormal;

	if (bInCoverMovLeft && RightInputValue > 0)
	{
		// 스피어 트레이스를 수행합니다.
		bool bHit = GetWorld()->SweepSingleByChannel(HitResult, Start, End, FQuat::Identity, ECC_GameTraceChannel1,
			FCollisionShape::MakeSphere(20.0f), QueryParams);

		// 디버그 드로잉
		if (TraceDebug != EDrawDebugTrace::None)
		{
			DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f, 0, 1.0f);
		}

		if (bHit)
		{
			float DotProduct = FVector::DotProduct(HitResult.ImpactNormal, CoverWallNormal);
			float AngleInDegrees = FMath::RadiansToDegrees(acosf(DotProduct));
			int TruncatedValue = FMath::TruncToInt(AngleInDegrees);

			if (MaxWalkInCoverAngle + 1 < (float)TruncatedValue)
			{
				WallLNormal = HitResult.ImpactNormal;

				{
					FVector Target = (WallLNormal * 30) + HitResult.ImpactPoint;
					float InterpSpeed = (InCoverTraceDistance / 20) * InCoverSpeedScalar;
					FVector NewLocation = FMath::VInterpTo(OwnerCharacter->GetActorLocation(), Target, GetWorld()->GetDeltaSeconds(), InterpSpeed);
					OwnerCharacter->SetActorLocation(NewLocation);
				}

				float Multiplier = bUseInvertedCoverNormal ? -1.0f : 1.0f;
				FVector Result = UKismetMathLibrary::Multiply_VectorFloat(WallLNormal, Multiplier);

				FVector MultipliedVector = UKismetMathLibrary::Multiply_VectorFloat(WallLNormal, Multiplier);
				FRotator RotFromVector = UKismetMathLibrary::Conv_VectorToRotator(MultipliedVector);

				FRotator NewRotation = UKismetMathLibrary::RInterpTo(OwnerCharacter->GetActorRotation(), RotFromVector, GetWorld()->GetDeltaSeconds(), 5);
				bool bSuccess = OwnerCharacter->SetActorRotation(NewRotation);

				bInCoverMovRight = false;
				bInCoverMovLeft = true;
				bInCoverIdleRight = true;

				if (bSuccess)
					CoverWallNormal = WallLNormal;
			}
			else
			{
				bInCoverMovRight = false;
			}
		}
	}
	else
	{
		bInCoverMovLeft = false;
	}
}

// 전방 중앙·좌·우 커버 탐지
void UCCoverSystemComponent::ForwardTracer()
{
	FHitResult HitResult;
	bool bCenterTraceHit = false;
	bool bRightTraceHit = false;
	bool bLeftTraceHit = false;
	bCanCover = false;

	float GetCoverMaxDistance = 100;
	FVector ForwardVector = UKismetMathLibrary::GetForwardVector(OwnerCharacter->GetActorRotation());

	ForwardVector.X = ForwardVector.X * GetCoverMaxDistance;
	ForwardVector.Y = ForwardVector.Y * GetCoverMaxDistance;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter); // 자신의 액터를 무시합니다.


	bCenterTraceHit = GetWorld()->SweepSingleByChannel(HitResult, OwnerCharacter->GetActorLocation(), OwnerCharacter->GetActorLocation()+ForwardVector,
		FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeSphere(20.0f), QueryParams);

	// 디버그 드로잉
	if (TraceDebug != EDrawDebugTrace::None)
	{
		DrawDebugLine(GetWorld(), OwnerCharacter->GetActorLocation(), OwnerCharacter->GetActorLocation() + ForwardVector, FColor::Red, false, 1.0f, 0, 1.0f);
	}

	if (bCenterTraceHit)
	{
		FVector StartR;
		FVector EndR;
		CalculateTracePoints(StartR, EndR, 30);
		bRightTraceHit = GetWorld()->SweepSingleByChannel(HitResult, StartR, EndR, FQuat::Identity,
			ECC_GameTraceChannel1, FCollisionShape::MakeSphere(20.0f), QueryParams);

			// 디버그 드로잉
			if (TraceDebug != EDrawDebugTrace::None)
			{
				DrawDebugLine(GetWorld(), StartR, EndR, FColor::Red, false, 1.0f, 0, 1.0f);
			}

		FVector StartL;
		FVector EndL;
		CalculateTracePoints(StartL, EndL, -30);
		bLeftTraceHit = GetWorld()->SweepSingleByChannel(HitResult, StartL, EndL, FQuat::Identity,
			ECC_GameTraceChannel1, FCollisionShape::MakeSphere(20.0f), QueryParams);

			// 디버그 드로잉
			if (TraceDebug != EDrawDebugTrace::None)
			{
				DrawDebugLine(GetWorld(), StartL, EndL, FColor::Red, false, 1.0f, 0, 1.0f);
			}


		bCanCover = bRightTraceHit && bLeftTraceHit && bCenterTraceHit;
		if (bCanCover)
		{
			FVector WallLocation = HitResult.Location;
			WallLocation.Z = OwnerCharacter->GetActorLocation().Z;

			CoverWallNormal = HitResult.Normal;
			CoverWallLocation = WallLocation;
		}
	}
}

void UCCoverSystemComponent::CalculateTracePoints(FVector & OutStart, FVector & OutEnd, float RightVectorMultiplier)
{
	float GetCoverMaxDistance = 100;

	FVector RightVector = UKismetMathLibrary::GetRightVector(OwnerCharacter->GetActorRotation()) * RightVectorMultiplier;
	FVector ForwardVector = UKismetMathLibrary::GetForwardVector(OwnerCharacter->GetActorRotation());

	ForwardVector.X = ForwardVector.X * GetCoverMaxDistance;
	ForwardVector.Y = ForwardVector.Y * GetCoverMaxDistance;

	OutStart = OwnerCharacter->GetActorLocation() + RightVector;
	OutEnd = OutStart + ForwardVector;


}

void UCCoverSystemComponent::SpeedUpdate(bool bSprinting)
{
	if (bSprinting)
	{
		InCoverSpeedScalar = 2;
	}
	else
	{
		InCoverSpeedScalar = 1;
	}
}

void UCCoverSystemComponent::CoverHeightTrace(FVector & OutStart, FVector & OutEnd)
{
	float Multiplier = bInCoverMovRight ? 30.0f : -30.0f;

	FVector Up = UKismetMathLibrary::GetUpVector(OwnerCharacter->GetActorRotation()) * 50;
	FVector Right = UKismetMathLibrary::GetRightVector(OwnerCharacter->GetActorRotation()) * Multiplier;
	FVector Forward = UKismetMathLibrary::GetForwardVector(OwnerCharacter->GetActorRotation()) * -100;

	OutStart = OwnerCharacter->GetActorLocation() + Up + Right;
	OutEnd = OutStart + Forward;
}

void UCCoverSystemComponent::CoverJumpRightTraceLocation(FVector & OutStart, FVector & OutEnd)
{
	FVector Right = UKismetMathLibrary::GetRightVector(OwnerCharacter->GetActorRotation()) * (InCoverTraceDistance + CoverToCoverMaxDistance);
	FVector Forward = UKismetMathLibrary::GetForwardVector(OwnerCharacter->GetActorRotation()) * -60;

	OutStart = OwnerCharacter->GetActorLocation() + Right;
	OutEnd = OutStart + Forward;
}

void UCCoverSystemComponent::CoverJumpLeftTraceLocation(FVector & OutStart, FVector & OutEnd)
{
	FVector Right = UKismetMathLibrary::GetRightVector(OwnerCharacter->GetActorRotation()) * ((InCoverTraceDistance + CoverToCoverMaxDistance) * -1);
	FVector Forward = UKismetMathLibrary::GetForwardVector(OwnerCharacter->GetActorRotation()) * -60;

	OutStart = OwnerCharacter->GetActorLocation() + Right;
	OutEnd = OutStart + Forward;
}

void UCCoverSystemComponent::CoverJumpingRightTracer()
{
	FVector Start, End;
	CoverJumpRightTraceLocation(Start, End);
	FHitResult HitResult;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter); // 자신의 액터를 무시합니다.


	bCanJumpToCoverLeft = GetWorld()->SweepSingleByChannel(HitResult, Start, End, FQuat::Identity,
		ECC_GameTraceChannel1, FCollisionShape::MakeCapsule(20, 40), QueryParams);
	// 디버그 드로잉
	if (TraceDebug != EDrawDebugTrace::None)
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f, 0, 1.0f);
	}

	if (bCanJumpToCoverLeft)
	{
		NewCoverNormal = HitResult.ImpactNormal;
		NewCoverLocation = HitResult.ImpactPoint;

		if (RightInputValue == -1)
		{
			if (bAutomaticallyCoverJump)
			{
				CoverToCoverJump(true);
			}
			else if(bRightInput)
			{
				CoverToCoverJump(true);
			}
		}
	}
}

void UCCoverSystemComponent::CoverJumpingLeftTracer()
{
	FVector Start, End;
	CoverJumpRightTraceLocation(Start, End);
	FHitResult HitResult;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter); // 자신의 액터를 무시합니다.

	bCanJumpToCoverRight = GetWorld()->SweepSingleByChannel(HitResult, Start, End, FQuat::Identity,
		ECC_GameTraceChannel1, FCollisionShape::MakeCapsule(20, 40), QueryParams);

	// 디버그 드로잉
	if (TraceDebug != EDrawDebugTrace::None)
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f, 0, 1.0f);
	}

	if (bCanJumpToCoverRight)
	{
		NewCoverNormal = HitResult.ImpactNormal;
		NewCoverLocation = HitResult.ImpactPoint;

		if (RightInputValue == 1)
		{
			if (bAutomaticallyCoverJump)
			{
				CoverToCoverJump(false);
			}
			else if (bRightInput)
			{
				CoverToCoverJump(false);
			}
		}
	}
}

void UCCoverSystemComponent::InCoverHeightCheck()
{
	 FCollisionQueryParams QueryParams;
	 QueryParams.AddIgnoredActor(OwnerCharacter); // 자신의 액터를 무시합니다.

	 FHitResult HitResult;

	 FVector Start, End;
	 CoverHeightTrace(Start, End);

	 // 세계가 유효한지 확인하고 라인 트레이스 실행
	 bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel1, QueryParams);

	 if (bHit)
	 {
		bInCoverCrouched = !bHit;
		bool PlayerCrouching;
		OwnerCharacter->GetCrouching(PlayerCrouching);

		if (PlayerCrouching != bInCoverCrouched && bInCoverCrouched)
		{
			OwnerCharacter->PlayerCrouchUpdate(true, false);
		}
	 }

}

void UCCoverSystemComponent::CoverLocation(FVector & OutLocation)
{
	FVector Location = CoverWallNormal * FVector(5, 5, 0);
	Location.X = Location.X + CoverWallLocation.X;
	Location.Y = Location.Y + CoverWallLocation.Y;
	Location.Z = OwnerCharacter->GetActorLocation().Z;

	OutLocation = Location;
}

void UCCoverSystemComponent::CoverJumpLocation(FVector & OutLocation)
{
	FVector Location = NewCoverNormal * FVector(22, 22, 0);
	Location.X = Location.X + NewCoverLocation.X;
	Location.Y = Location.Y + NewCoverLocation.Y;
	Location.Z = OwnerCharacter->GetActorLocation().Z;

	OutLocation = Location;
}

void UCCoverSystemComponent::SideTracers()
{
	// 오른쪽
	FVector RightArrowLocation, RightArrowForward, RightArrowRight, RightArrowUp;
	SimulateRightArrow(RightArrowLocation, RightArrowForward, RightArrowRight, RightArrowUp);
	FVector RightEnd = RightArrowLocation + (RightArrowForward * -100);

	FHitResult RightHitResult;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter); // 자신의 액터를 무시합니다.


	bInCoverMovRight = GetWorld()->SweepSingleByChannel(RightHitResult, RightArrowLocation, RightEnd, FQuat::Identity,
		ECC_GameTraceChannel1,	FCollisionShape::MakeCapsule(20, 60), QueryParams);

	// 디버그 드로잉
	if (TraceDebug != EDrawDebugTrace::None)
	{
		DrawDebugLine(GetWorld(), RightArrowLocation, RightEnd, FColor::Red, false, 1.0f, 0, 1.0f);
	}

	FTimerHandle RightHandle;
	GetWorld()->GetTimerManager().SetTimer(RightHandle, this, &UCCoverSystemComponent::SideTracerRightTimerFunc, 0, false, 0.2f);

	// 왼쪽
	FVector LeftArrowLocation, LeftArrowForward, LeftArrowLeft, LeftArrowUp;
	SimulateLeftArrow(LeftArrowLocation, LeftArrowForward, LeftArrowLeft, LeftArrowUp);
	FVector LeftEnd = LeftArrowLocation + (LeftArrowForward * -100);

	FHitResult LeftHitResult;

	bInCoverMovLeft = GetWorld()->SweepSingleByChannel(LeftHitResult, LeftArrowLocation, LeftEnd, FQuat::Identity,
		ECC_GameTraceChannel1, FCollisionShape::MakeCapsule(20, 60), QueryParams);

	// 디버그 드로잉
	if (TraceDebug != EDrawDebugTrace::None)
	{
		DrawDebugLine(GetWorld(), LeftArrowLocation, LeftEnd, FColor::Red, false, 1.0f, 0, 1.0f);
	}

	FTimerHandle LeftHandle;
	GetWorld()->GetTimerManager().SetTimer(LeftHandle, this, &UCCoverSystemComponent::SideTracerLeftTimerFunc, 0, false, 0.2f);
}

void UCCoverSystemComponent::CoverToCoverJump(bool bInRight)
{
	bool AnimCrouch;
	AnimInstance->CoverGetIsCrouched(AnimCrouch);
	UAnimMontage* MontageToPlay = nullptr;

	if (AnimCrouch)
	{
		MontageToPlay = bInRight ? CrouchCoverToCoverRightMontage : CrouchCoverToCoverLeftMontage;
	}
	else
	{
		MontageToPlay = bInRight ? CoverToCoverRightMontage : CoverToCoverLeftMontage;
	}

	AnimInstance->Montage_Play(MontageToPlay, 1.5f, EMontagePlayReturnType::MontageLength, 0.2f, true);

	bJumpingCoverToCover = true;

	FVector TargetLocation;
	CoverJumpLocation(TargetLocation);

	FVector NewCover = NewCoverNormal;
	NewCover.Z = 0;

	FRotator TargetRotation = NewCover.Rotation();


	UKismetSystemLibrary::MoveComponentTo(OwnerCharacter->GetCapsuleComponent(),
		TargetLocation,	TargetRotation,	false,	false,	0.7f, false, EMoveComponentAction::Move, FLatentActionInfo());

	bJumpingCoverToCover = false;
	bCanJumpToCoverLeft = false;
	bCanJumpToCoverRight = false;

}

void UCCoverSystemComponent::TakeCover()
{

	ICoverInterface* CoverInterface = Cast<ICoverInterface>(this);
	if (bCanCover && !bInCover)
	{
		if (GetInCoverMontage)
		{
			AnimInstance->CoverUpdate(true);
		}
		else
		{
			CoverInterface->CoverUpdate(true);
		}

		if (OwnerCharacter->GetCharacterMovement()->IsCrouching())
		{
			OwnerCharacter->UnCrouch();
			UKismetSystemLibrary::Delay(GetWorld(), 0.1f, FLatentActionInfo());
		}
		else
		{
			FVector NewWallNormal = CoverWallNormal;
			NewWallNormal.Z = 0;
			float Multiplier = bUseInvertedCoverNormal ? -1.0f : 1.0f;
			FVector MultipliedVector = UKismetMathLibrary::Multiply_VectorFloat(NewWallNormal, Multiplier);
			FRotator NewRotation = UKismetMathLibrary::Conv_VectorToRotator(MultipliedVector);

			OwnerCharacter->SetActorRotation(NewRotation);

			FVector NewLocation;
			CoverLocation(NewLocation);
			OwnerCharacter->SetActorLocation(NewLocation);

			OwnerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

			bInCover = true;
			bRestoreCollisionTest = SpringArm->bDoCollisionTest;
		}
	}

}


void UCCoverSystemComponent::GetCoverMontages(UAnimMontage*& In, UAnimMontage*& Out, UAnimMontage*& InCrouched, UAnimMontage*& OutCrouched)
{
	In = GetInCoverMontage;
	Out = GetOutCoverMontage;
	InCrouched = CrouchGetInCoverMontage;
	OutCrouched = CrouchGetOutCoverMontage;

}

void UCCoverSystemComponent::CSSync(FCoverSystem & CSStruct)
{
	CSStruct.bInCover = bInCover;
	CSStruct.bCanMoveLeft = bInCoverMovLeft;
	CSStruct.bCanMoveRight = bInCoverMovRight;
	CSStruct.bCanCover = bCanCover;

}

void UCCoverSystemComponent::TSSync(FTargetingSystem & TSStruct)
{
}

void UCCoverSystemComponent::GHSync(FGrappingHookSystem & GHStruct)
{
}

void UCCoverSystemComponent::NCSSync(FNCSSystem & NCSStruct)
{
}

void UCCoverSystemComponent::SLSSync(FLedgeSystem & SLSStruct)
{
}



void UCCoverSystemComponent::CoverExit()
{
	ExitCover();
}

void UCCoverSystemComponent::StealthKill()
{
	bCameraEdgeMove = false;
	bCameraMoving = true;
	SetCameraMoverTimer(0.01f);
}

void UCCoverSystemComponent::InputBlocked(bool & Blocked)
{
	bIsInputBlocked = Blocked;
}

void UCCoverSystemComponent::CoverGetIsCrouched(bool & OutbCrouched)
{
	OutbCrouched = bInCoverCrouched;
}

void UCCoverSystemComponent::CoverUpdate(bool bCoverUpdate)
{
	bInCover = bCoverUpdate;
}

void UCCoverSystemComponent::GetCoverStatus(bool & InCover, bool & InCoverMovRight, bool & InCoverMovLeft, bool & InCoverIdleRight)
{
	InCover = bInCover;
	InCoverMovRight = bInCoverMovRight;
	InCoverMovLeft = bInCoverMovLeft;
	InCoverIdleRight = bInCoverIdleRight;
}

void UCCoverSystemComponent::ExitCover()
{
	ICoverInterface* CoverInterface = Cast<ICoverInterface>(this);

	if (GetOutCoverMontage)
	{
		AnimInstance->CoverUpdate(false);
	}
	else
	{
		CoverInterface->CoverUpdate(false);
	}

	bInCoverMovLeft = false;
	bInCoverMovRight = false;
	bCameraEdgeMove = false;
	bCameraMoving = true;

	OwnerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SpeedUpdate(false);

	SetCameraMoverTimer(0.01f);

	UKismetSystemLibrary::Delay(this, 0.5f, FLatentActionInfo());

	OwnerCharacter->SetIsSprinting(false);

	bool PlayerCrouch;
	OwnerCharacter->GetCrouching(PlayerCrouch);
	if (!PlayerCrouch)
	{
		if (OwnerCharacter->GetCharacterMovement()->IsCrouching() == true)
		{
			OwnerCharacter->UnCrouch();
		}
	}
}

void UCCoverSystemComponent::InCoverExitToGrab()
{
	ExitCover();

	UKismetSystemLibrary::Delay(this, 0.5f, FLatentActionInfo());
	FRotator NewRotation = OwnerCharacter->GetActorRotation();
	NewRotation.Yaw = NewRotation.Yaw + 180;

	OwnerCharacter->SetActorRotation(NewRotation);

	OwnerCharacter->Jump();
}

void UCCoverSystemComponent::SideTracerRightTimerFunc()
{
	if (!bCanJumpToCoverLeft && !bCanJumpToCoverRight && !bJumpingCoverToCover)
	{
		if (bCameraZoomOnEdge)
		{
			CameraEdgeZoom(false);
		}
	}
}

void UCCoverSystemComponent::SideTracerLeftTimerFunc()
{
	if (!bCanJumpToCoverLeft && !bCanJumpToCoverRight && !bJumpingCoverToCover)
	{
		if (bCameraZoomOnEdge)
		{
			CameraEdgeZoom(true);
		}
	}
}

void UCCoverSystemComponent::CameraEdgeZoom(bool bRight)
{
	float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
	if (bRight)
	{
		if (!bCameraMoving)
		{
			if (bInCoverMovLeft)
			{
				if (bInCoverMovLeft && bInCoverMovRight && bCameraEdgeMove)
				{
					bCameraEdgeMove = false;
					bCameraMoving = true;
					SetCameraMoverTimer(DeltaTime);
				}
			}
			else
			{
				if (!bCameraEdgeMove)
				{
					bCameraEdgeMove = true;
					bCameraMoving = true;
					bZoomRight = true;
					SetCameraMoverTimer(DeltaTime);
				}
			}

		}
	}
	else
	{
		if (!bCameraMoving)
		{
			if (bInCoverIdleRight)
			{
				if (bInCoverMovLeft && bInCoverMovRight && bCameraEdgeMove)
				{
					bCameraEdgeMove = false;
					bCameraMoving = true;
					SetCameraMoverTimer(DeltaTime);
				}
			}
			else
			{
				if (!bCameraEdgeMove)
				{
					bCameraEdgeMove = true;
					bCameraMoving = true;
					bZoomRight = false;
					SetCameraMoverTimer(DeltaTime);
				}
			}
		}
	}


}

void UCCoverSystemComponent::SetCameraMoverTimer(float Time)
{

	GetWorld()->GetTimerManager().SetTimer(CameraMoveTimer, this, &UCCoverSystemComponent::CameraMove, Time, true);
}

void UCCoverSystemComponent::CameraMove()
{
	float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
	FVector ZoomRightVector = bZoomRight ? FVector(200, 100, 50) : FVector(200, -100, 50);
	FVector CameraEdgeMoveVector = bCameraEdgeMove ? ZoomRightVector : FVector(0);

	FVector NewLocation = FMath::VInterpTo(SpringArm->SocketOffset, CameraEdgeMoveVector, DeltaTime, DeltaTime / 0.3f);

	if (SpringArm)
	{
		if (!bInCover)
			bCameraEdgeMove = false;

		if (bCameraMoving)
		{
			SpringArm->SocketOffset = NewLocation;
			bCameraMoving = !SpringArm->SocketOffset.Equals(NewLocation, 0.001f);
		}
		else
		{
			GetWorld()->GetTimerManager().ClearTimer(CameraMoveTimer);
		}
	}
}

void UCCoverSystemComponent::ProcessRightMovement(float& bRightInputValue)
{
	if (bIsInputBlocked)
	{
		CoverRightInputValue = 0;
	}
	else
	{
		CoverRightInputValue = bRightInputValue;
		if (CoverRightInputValue != 0.0f)
		{
			if (!bRightDoOnceCompleted)
			{
				bRightInput = true;

				FTimerHandle Handle;
				FTimerDelegate TimerDelegate;
				TimerDelegate.BindLambda([this]()
					{
						bRightInput = false;
					});

				GetWorld()->GetTimerManager().SetTimer(Handle, TimerDelegate, 0.2f, false);

				bRightDoOnceCompleted = true;
			}

		}
		else
		{
			bRightInput = false;
			bRightDoOnceCompleted = false;
		}
	}
}

void UCCoverSystemComponent::ProcessForwardMovement(float& bForwardInputValue)
{
	if (bIsInputBlocked)
	{
		CoverForwardInputValue = 0;
	}
	else
	{
		CoverForwardInputValue = bForwardInputValue;
		if (UKismetMathLibrary::InRange_FloatFloat(CoverForwardInputValue, -1.0f, -0.9f, true, true))
		{
			if (bInCover)
			{
				if (!bForwardDoOnceCompleted)
				{
					if (!bUseKeyToExitCover)
					{
						ExitCover();
						bForwardDoOnceCompleted = true;
					}
				}
			}
			else
			{
				bForwardDoOnceCompleted = false;
			}
		}
		else
		{
			if (CoverForwardInputValue == 0.0f)
			{
				bForwardDoOnceCompleted = false;
			}
		}
	}
}
