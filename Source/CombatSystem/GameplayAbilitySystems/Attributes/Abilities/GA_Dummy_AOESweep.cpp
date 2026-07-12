// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Dummy_AOESweep.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameplayTagContainer.h"

void UGA_DummyAOESweep::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (AttackMontage)
	{
		// Play attack animation
		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, NAME_None, AttackMontage, 1.f);
		MontageTask->OnCompleted.AddDynamic(this, &UGA_DummyAOESweep::OnAttackMontageEnded);
		MontageTask->OnInterrupted.AddDynamic(this, &UGA_DummyAOESweep::OnAttackMontageEnded);
		MontageTask->ReadyForActivation();

		// Wait for the AnimNotify to fire mid animation and do the actual trace
		WaitHitWindowTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this, FGameplayTag::RequestGameplayTag(FName("Event.Melee.HitWindow")));
		WaitHitWindowTask->EventReceived.AddDynamic(this, &UGA_DummyAOESweep::OnHitWindowReceived);
		WaitHitWindowTask->ReadyForActivation();
	}
	else
	{
		// No montage assigned, end immediately 
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void UGA_DummyAOESweep::OnHitWindowReceived(FGameplayEventData Payload)
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar) return;

	TArray<FHitResult> Hits;
	TArray<AActor*> Ignore;
	Ignore.Add(Avatar);

	UKismetSystemLibrary::SphereTraceMulti(
		Avatar, Avatar->GetActorLocation(), Avatar->GetActorLocation(), Radius,
		UEngineTypes::ConvertToTraceType(ECC_Pawn), false, Ignore,
		EDrawDebugTrace::ForDuration, Hits, true);

	TSet<AActor*> AlreadyHit;
	for (const FHitResult& Hit : Hits)
	{
		if (Hit.GetActor() && !AlreadyHit.Contains(Hit.GetActor()))
		{
			AlreadyHit.Add(Hit.GetActor());
			ApplyDamageToTarget(Hit.GetActor(), Damage);
		}
	}
}

void UGA_DummyAOESweep::OnAttackMontageEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}