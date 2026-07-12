// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Dummy_AOESweep.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

void UGA_DummyAOESweep::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar) { EndAbility(Handle, ActorInfo, ActivationInfo, true, true); return; }

	TArray<FHitResult> Hits;
	TArray<AActor*> Ignore; Ignore.Add(Avatar);

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
	if (AttackMontage)
	{
		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, NAME_None, AttackMontage, 1.f);
		MontageTask->OnCompleted.AddDynamic(this, &UGA_DummyAOESweep::OnAttackMontageEnded);
		MontageTask->OnInterrupted.AddDynamic(this, &UGA_DummyAOESweep::OnAttackMontageEnded);
		MontageTask->ReadyForActivation();
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void UGA_DummyAOESweep::OnAttackMontageEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
