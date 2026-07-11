// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_DashAttack.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

UGA_DashAttack::UGA_DashAttack()
{
	ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Stance.Melee")));
	// CooldownGameplayEffectClass set in the _BP child
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

	FVector DashDirection = Character->GetActorForwardVector();
	FVector DashVelocity = DashDirection * (DashDistance / DashDuration);

	Character->GetCharacterMovement()->Launch(DashVelocity);

	// Trace along the dash path for anything to hit
	FVector Start = Character->GetActorLocation();
	FVector End = Start + DashDirection * DashDistance;

	TArray<FHitResult> Hits;
	TArray<AActor*> Ignore;
	Ignore.Add(Character);

	UKismetSystemLibrary::SphereTraceMulti(
		Character, Start, End, 80.f,
		UEngineTypes::ConvertToTraceType(ECC_Pawn),
		false, Ignore, EDrawDebugTrace::ForDuration, Hits, true);

	TSet<AActor*> AlreadyHit;
	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && !AlreadyHit.Contains(HitActor))
		{
			AlreadyHit.Add(HitActor);
			ApplyDamageToTarget(HitActor, Stats.Damage);
		}
	}
	if (DashMontage)
	{
		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, NAME_None, DashMontage, 1.f);
		MontageTask->OnCompleted.AddDynamic(this, &UGA_DashAttack::OnDashMontageEnded);
		MontageTask->OnInterrupted.AddDynamic(this, &UGA_DashAttack::OnDashMontageEnded);
		MontageTask->ReadyForActivation();
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void UGA_DashAttack::OnDashMontageEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
