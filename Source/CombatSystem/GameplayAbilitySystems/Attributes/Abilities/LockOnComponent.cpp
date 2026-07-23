// Fill out your copyright notice in the Description page of Project Settings.

#include "LockOnComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "../../Characters/CharacterBase.h"

ULockOnComponent::ULockOnComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULockOnComponent::ToggleLockOn()
{
	if (IsLockedOn())
	{
		ClearLockOn();
	}
	else
	{
		AActor* Target = FindBestTarget();
		if (Target)
		{
			CurrentTarget = Target;
			OnLockOnTargetChanged.Broadcast(Target);
		}
	}
}

void ULockOnComponent::ClearLockOn()
{
	CurrentTarget = nullptr;
	OnLockOnTargetChanged.Broadcast(nullptr);
}

AActor* ULockOnComponent::FindBestTarget() const
{
	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (!OwnerChar) return nullptr;

	FVector Start = OwnerChar->GetActorLocation();
	FVector End = Start + OwnerChar->GetActorForwardVector() * LockOnRange;

	TArray<FHitResult> Hits;
	TArray<AActor*> Ignore;
	Ignore.Add(OwnerChar);

	UKismetSystemLibrary::SphereTraceMulti(
		OwnerChar, Start, End, LockOnSearchRadius,
		UEngineTypes::ConvertToTraceType(ECC_Pawn),
		false, Ignore, EDrawDebugTrace::None, Hits, true);

	for (const FHitResult& Hit : Hits)
	{
		if (Hit.GetActor() && Cast<ACharacterBase>(Hit.GetActor()))
		{
			return Hit.GetActor();
		}
	}
	return nullptr;
}

void ULockOnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Auto clear if the target dies or becomes invalid 
	if (CurrentTarget.IsValid())
	{
		if (ACharacterBase* TargetBase = Cast<ACharacterBase>(CurrentTarget.Get()))
		{
			
			float Distance = FVector::Dist(GetOwner()->GetActorLocation(), CurrentTarget->GetActorLocation());
			if (Distance > LockOnRange * 1.5f) // some leeway beyond initial range before auto unlocking
			{
				ClearLockOn();
			}
		}
	}
}

