// Fill out your copyright notice in the Description page of Project Settings.

#include "GA_MeleeCombo.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemComponent.h"

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

	UE_LOG(LogTemp, Warning, TEXT("Melee trace hit %d actors"), Hits.Num());

	for (const FHitResult& Hit : Hits)
	{
		if (Hit.GetActor())
		{
			float Damage = bIsCritical ? BaseDamage * 2.f : BaseDamage;
			ApplyDamageToTarget(Hit.GetActor(), Damage);
		}
	}
}