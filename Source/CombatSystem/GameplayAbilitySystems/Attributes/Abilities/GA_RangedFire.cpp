// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_RangedFire.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../CombatAttributeSet.h"

void UGA_RangedFire::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Ammo check happens on the Character side before this ability is even activated (Step 5)

	FVector Start = Character->GetPawnViewLocation();
	FVector End = Start + Character->GetControlRotation().Vector() * TraceRange;

	FHitResult Hit;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Character);

	bool bHit = UKismetSystemLibrary::LineTraceSingle(
		Character, Start, End, UEngineTypes::ConvertToTraceType(ECC_Pawn),
		false, ActorsToIgnore, EDrawDebugTrace::ForDuration, Hit, true);

	if (bHit && Hit.GetActor())
	{
		ApplyDamageToTarget(Hit.GetActor(), BaseDamage);
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
