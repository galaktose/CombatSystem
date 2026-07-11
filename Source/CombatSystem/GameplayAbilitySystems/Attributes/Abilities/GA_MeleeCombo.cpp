// Fill out your copyright notice in the Description page of Project Settings.

#include "GA_MeleeCombo.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemComponent.h"
#include "../../Characters/Combat_Player.h"

UGA_MeleeCombo::UGA_MeleeCombo()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_MeleeCombo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// TriggerEventData->EventTag will carry which combo stage (Ability.Melee.Combo1/2/3) triggered this
	bool bIsCritical = TriggerEventData &&
		TriggerEventData->EventTag == FGameplayTag::RequestGameplayTag(FName("Ability.Melee.Combo3"));

	DoMeleeTrace(bIsCritical);

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_MeleeCombo::DoMeleeTrace(bool bIsCritical)
{
	//UE_LOG(LogTemp, Warning, TEXT("DoMeleeTrace"));
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character) return;

	FVector Start = Character->GetActorLocation() + Character->GetActorForwardVector() * 50.f;
	FVector End = Start + Character->GetActorForwardVector() * TraceRange;

	TArray<FHitResult> Hits;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Character);

	UKismetSystemLibrary::SphereTraceMulti(
		Character, Start, End, TraceRadius,
		UEngineTypes::ConvertToTraceType(ECC_Pawn),
		false, ActorsToIgnore, EDrawDebugTrace::ForDuration,
		Hits, true);

	TSet<AActor*> AlreadyHit; // tracks unique actors this trace to prevent multiple damage applications in a single trace

	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && !AlreadyHit.Contains(HitActor))
		{
			AlreadyHit.Add(HitActor);
			float Damage = bIsCritical ? BaseDamage * 2.f : BaseDamage;
			ApplyDamageToTarget(HitActor, Damage);
			if (ACombat_Player* Player = Cast<ACombat_Player>(Character))
			{
				Player->LastHitTarget = HitActor;

				if (bIsCritical) // only broadcast when this specific hit was a crit
				{
					Player->OnCriticalHitLanded.Broadcast();
				}
			}

			//UE_LOG(LogTemp, Warning, TEXT("Applied %.1f damage to %s"), Damage, *HitActor->GetName());
		}
	}
}