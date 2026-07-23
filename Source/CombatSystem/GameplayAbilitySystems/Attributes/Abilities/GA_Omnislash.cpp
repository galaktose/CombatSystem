// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_OmniSlash.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemComponent.h"
#include "LockOnComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "../../Characters/Combat_Player.h"

UGA_OmniSlash::UGA_OmniSlash()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Stance.Melee")));
}

void UGA_OmniSlash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
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

	ACombat_Player* Player = Cast<ACombat_Player>(Character);
	if (!Player || !Player->LockOnComponent || !Player->LockOnComponent->IsLockedOn())
	{
		// Omnislash requires being locked on
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	LockedTarget = Player->LockOnComponent->GetLockedTarget();

	if (!LockedTarget.IsValid())
	{
		// No valid target found, end immediately
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	CurrentHitIndex = 0;

	Player->SetAttacking(true);

	WaitHitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, FGameplayTag::RequestGameplayTag(FName("Event.OmniSlash.Hit")), nullptr, false, true);
	WaitHitEventTask->EventReceived.AddDynamic(this, &UGA_OmniSlash::OnHitEventReceived);
	WaitHitEventTask->ReadyForActivation();

	if (OmniSlashMontage)
	{
		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, NAME_None, OmniSlashMontage, 1.f);
		MontageTask->OnCompleted.AddDynamic(this, &UGA_OmniSlash::OnOmniSlashMontageEnded);
		MontageTask->OnInterrupted.AddDynamic(this, &UGA_OmniSlash::OnOmniSlashMontageEnded);
		MontageTask->ReadyForActivation();
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void UGA_OmniSlash::OnHitEventReceived(FGameplayEventData Payload)
{
	CurrentHitIndex++;
	bool bIsFinalHit = (CurrentHitIndex >= TotalHits);
	PerformStrike(bIsFinalHit);
}

void UGA_OmniSlash::PerformStrike(bool bIsFinalHit)
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character || !LockedTarget.IsValid()) return;

	AActor* Target = LockedTarget.Get();
	FVector TargetLocation = Target->GetActorLocation();

	// Vary the strike angle per hit for visual flourish, orbit around the target
	float AngleOffsetDegrees = (CurrentHitIndex - 1) * 45.f; // spreads 8 hits around a circle
	FVector OffsetDirection = FRotator(0.f, AngleOffsetDegrees, 0.f).RotateVector(FVector(1.f, 0.f, 0.f));
	FVector DesiredStrikePosition = TargetLocation + OffsetDirection * StrikeDistanceFromTarget;

	FHitResult TraceHit;
	TArray<AActor*> Ignore;
	Ignore.Add(Character);
	Ignore.Add(Target);

	bool bBlocked = UKismetSystemLibrary::CapsuleTraceSingle(
		Character,
		Character->GetActorLocation(), DesiredStrikePosition,
		Character->GetCapsuleComponent()->GetScaledCapsuleRadius(),
		Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false, Ignore, EDrawDebugTrace::ForDuration, TraceHit, true);

	FVector FinalPosition = bBlocked ? TraceHit.Location : DesiredStrikePosition;

	Character->SetActorLocation(FinalPosition, false, nullptr, ETeleportType::TeleportPhysics);

	// Face the target after teleporting
	FVector LookDirection = (TargetLocation - FinalPosition).GetSafeNormal();
	Character->SetActorRotation(LookDirection.Rotation());

	// Interrupt the target's current ability (e.g. cancels a mid-windup combo attack)
	if (ACharacterBase* TargetBase = Cast<ACharacterBase>(Target))
	{
		if (TargetBase->GetAbilitySystemComponent())
		{
			TargetBase->GetAbilitySystemComponent()->CancelAbilities(); // cancels any currently active ability on the target
		}
	}

	float Damage = bIsFinalHit ? DamagePerHit * FinalHitDamageMultiplier : DamagePerHit;
	ApplyDamageToTarget(Target, Damage);

	OnHitLanded(Target, Damage, bIsFinalHit); // hook point - for stagger if got time
}

void UGA_OmniSlash::OnHitLanded(AActor* HitActor, float DamageDealt, bool bIsFinalHit)
{
	// Currently empty beyond logging - place for future Stagger implementation if got time
	// meter increment would be added: e.g. Cast<ACharacterBase>(HitActor)->AddStagger(DamageDealt * 0.5f);
	UE_LOG(LogTemp, Warning, TEXT("OmniSlash hit %d/%d on %s, damage: %.1f, final: %s"),
		CurrentHitIndex, TotalHits, *HitActor->GetName(), DamageDealt, bIsFinalHit ? TEXT("true") : TEXT("false"));
}

void UGA_OmniSlash::OnOmniSlashMontageEnded()
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (ACombat_Player* Player = Cast<ACombat_Player>(Character))
	{
		Player->SetAttacking(false);
	}

	LockedTarget = nullptr;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

