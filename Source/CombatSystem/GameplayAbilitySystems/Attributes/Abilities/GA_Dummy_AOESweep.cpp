// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Dummy_AOESweep.h"
#include "Kismet/KismetSystemLibrary.h"

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
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
