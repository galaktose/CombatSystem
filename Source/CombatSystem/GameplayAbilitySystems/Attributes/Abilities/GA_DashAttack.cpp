// Fill out your copyright notice in the Description page of Project Settings.
#pragma	once
#include "GA_DashAttack.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

UGA_DashAttack::UGA_DashAttack()
{
	ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Stance.Melee")));
}

void UGA_DashAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (DashMontage)
	{
		// Play only the wind-up section first, wait for it to finish
		UAbilityTask_PlayMontageAndWait* StartTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, NAME_None, DashMontage, 1.f, FName("DashStart"));
		StartTask->OnCompleted.AddDynamic(this, &UGA_DashAttack::OnDashStartMontageEnded);
		StartTask->OnInterrupted.AddDynamic(this, &UGA_DashAttack::OnDashMontageEnded); // bail out cleanly if interrupted mid-windup
		StartTask->ReadyForActivation();
	}
	else
	{
		// No montage assigned - just do the dash immediately (fallback)
		PerformDashMovement();
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void UGA_DashAttack::OnDashStartMontageEnded()
{
	// Wind-up finished, now teleport
	PerformDashMovement();

	// Play DashFollowThrough section
	if (DashMontage)
	{
		UAbilityTask_PlayMontageAndWait* FollowThroughTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, NAME_None, DashMontage, 1.f, FName("DashFollowThrough"));
		FollowThroughTask->OnCompleted.AddDynamic(this, &UGA_DashAttack::OnDashMontageEnded);
		FollowThroughTask->OnInterrupted.AddDynamic(this, &UGA_DashAttack::OnDashMontageEnded);
		FollowThroughTask->ReadyForActivation();
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UGA_DashAttack::PerformDashMovement()
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character) return;

	FVector DashDirection = Character->GetActorForwardVector();
	FVector StartLocation = Character->GetActorLocation();
	FVector DesiredEnd = StartLocation + DashDirection * DashDistance;

	FHitResult TraceHit;
	TArray<AActor*> Ignore;
	Ignore.Add(Character);

	bool bBlocked = UKismetSystemLibrary::CapsuleTraceSingle(
		Character,
		StartLocation, DesiredEnd,
		Character->GetCapsuleComponent()->GetScaledCapsuleRadius(),
		Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		Ignore,
		EDrawDebugTrace::ForDuration,
		TraceHit,
		true);

	FVector FinalLocation = bBlocked ? TraceHit.Location : DesiredEnd;

	TArray<FHitResult> DamageHits;
	UKismetSystemLibrary::SphereTraceMulti(
		Character, StartLocation, FinalLocation, 80.f,
		UEngineTypes::ConvertToTraceType(ECC_Pawn),
		false, Ignore, EDrawDebugTrace::ForDuration, DamageHits, true);

	TSet<AActor*> AlreadyHit;
	for (const FHitResult& Hit : DamageHits)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && !AlreadyHit.Contains(HitActor))
		{
			AlreadyHit.Add(HitActor);
			ApplyDamageToTarget(HitActor, Stats.Damage);
		}
	}

	Character->SetActorLocation(FinalLocation, false, nullptr, ETeleportType::TeleportPhysics);
}

void UGA_DashAttack::OnDashMontageEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}