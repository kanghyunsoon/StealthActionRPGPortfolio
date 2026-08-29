#include "Components/CFeetComponent.h"
#include "Global.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"

UCFeetComponent::UCFeetComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UCFeetComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());
}

void UCFeetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	float leftDistance, rightDistance;
	FRotator leftRotator, rightRotator;

	Trace(LeftSocketName, leftDistance, leftRotator);
	Trace(RightSocketName, rightDistance, rightRotator);

	float offset = FMath::Min(leftDistance, rightDistance);

	Data.PelvisDistance.Z = UKismetMathLibrary::FInterpTo(Data.PelvisDistance.Z, offset, DeltaTime, InterpSpeed);

	Data.LeftDistance.X = UKismetMathLibrary::FInterpTo(Data.LeftDistance.X, (leftDistance - offset), DeltaTime, InterpSpeed);
	Data.RightDistance.X = UKismetMathLibrary::FInterpTo(Data.RightDistance.X, ((rightDistance - offset) * -1.0f), DeltaTime, InterpSpeed);
	Data.LeftRotation = leftRotator;
	Data.RightRotation = rightRotator;
}

void UCFeetComponent::Trace(FName InName, float& OutDistance, FRotator& OutRotation)
{
	FVector socket = OwnerCharacter->GetMesh()->GetSocketLocation(InName);

	float z_start = OwnerCharacter->GetActorLocation().Z;
	FVector start = FVector(socket.X, socket.Y, z_start);

	float z_end = start.Z - OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - TraceDistance;
	FVector end = FVector(socket.X, socket.Y, z_end);

	TArray<AActor*> ignores;
	ignores.Add(OwnerCharacter);

	FHitResult hitResult;
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), start, end, ETraceTypeQuery::TraceTypeQuery1,
		true, ignores, DrawDebug, hitResult, true, FLinearColor::Green, FLinearColor::Red);

	OutDistance = 0.0f;
	OutRotation = FRotator::ZeroRotator;

	CheckFalse(hitResult.bBlockingHit);

	float length = (hitResult.ImpactPoint - hitResult.TraceEnd).Size();
	OutDistance = length + OffsetDistance - TraceDistance;

	float roll = UKismetMathLibrary::DegAtan2(hitResult.Normal.Y, hitResult.Normal.Z);
	float pitch = (UKismetMathLibrary::DegAtan2(hitResult.Normal.X, hitResult.Normal.Z)) * -1.0f;

	OutRotation = FRotator(pitch, 0, roll);
}
