// Fill out your copyright notice in the Description page of Project Settings.

#include "GA_MeleeCombo.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemComponent.h"
#include "../../Characters/Combat_Player.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Kismet/GameplayStatics.h"

UGA_MeleeCombo::UGA_MeleeCombo()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_MeleeCombo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());

	bool bIsCritical = TriggerEventData &&
		TriggerEventData->EventTag == FGameplayTag::RequestGameplayTag(FName("Ability.Melee.Combo3"));

	FName SectionName = NAME_None;
	int32 ComboStageIndex = 0; // index matching sound array slots

	if (TriggerEventData->EventTag == FGameplayTag::RequestGameplayTag(FName("Ability.Melee.Combo1")))
	{
		SectionName = FName("Combo1");
		ComboStageIndex = 0; 
	}
	else if (TriggerEventData->EventTag == FGameplayTag::RequestGameplayTag(FName("Ability.Melee.Combo2")))
	{
		SectionName = FName("Combo2");
		ComboStageIndex = 1; 
	}
	else if (TriggerEventData->EventTag == FGameplayTag::RequestGameplayTag(FName("Ability.Melee.Combo3")))
	{
		SectionName = FName("Combo3");
		ComboStageIndex = 2; 
	}

	// face camera direction (from earlier fix)
	if (Character)
	{
		if (APlayerController* PC = Cast<APlayerController>(Character->GetController()))
		{
			FRotator CamRotation = PC->GetControlRotation();
			FRotator FacingRotation(0.f, CamRotation.Yaw, 0.f);
			Character->SetActorRotation(FacingRotation);
		}
	}

	DoMeleeTrace(bIsCritical, ComboStageIndex); // pass stage index

	if (ACombat_Player* Player = Cast<ACombat_Player>(Character))
	{
		Player->SetAttacking(true);
		if (StoredMaxWalkSpeed <= 0.f)
		{
			StoredMaxWalkSpeed = Player->GetCharacterMovement()->MaxWalkSpeed;
		}
		Player->GetCharacterMovement()->MaxWalkSpeed = 0.f;
	}

	if (ComboMontage && SectionName != NAME_None)
	{
		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, NAME_None, ComboMontage, 1.f, SectionName);
		MontageTask->OnCompleted.AddDynamic(this, &UGA_MeleeCombo::OnComboMontageEnded);
		MontageTask->OnInterrupted.AddDynamic(this, &UGA_MeleeCombo::OnComboMontageEnded);
		MontageTask->OnCancelled.AddDynamic(this, &UGA_MeleeCombo::OnComboMontageEnded);
		MontageTask->ReadyForActivation();
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void UGA_MeleeCombo::DoMeleeTrace(bool bIsCritical, int32 ComboStageIndex) // MODIFIED signature
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

	TSet<AActor*> AlreadyHit;
	bool bAnyHitLanded = false;

	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && !AlreadyHit.Contains(HitActor))
		{
			AlreadyHit.Add(HitActor);
			bAnyHitLanded = true;

			float Damage = bIsCritical ? BaseDamage * 2.f : BaseDamage;
			ApplyDamageToTarget(HitActor, Damage);

			if (ACombat_Player* Player = Cast<ACombat_Player>(Character))
			{
				Player->LastHitTarget = HitActor;
				if (bIsCritical)
				{
					Player->OnCriticalHitLanded.Broadcast();
				}
			}
		}
	}

	// MODIFIED — pick the sound matching this specific combo stage
	USoundBase* SoundToPlay = nullptr;
	if (bAnyHitLanded)
	{
		if (ComboHitSounds.IsValidIndex(ComboStageIndex))
		{
			SoundToPlay = ComboHitSounds[ComboStageIndex];
		}
	}
	else
	{
		if (ComboMissSounds.IsValidIndex(ComboStageIndex))
		{
			SoundToPlay = ComboMissSounds[ComboStageIndex];
		}
	}

	if (SoundToPlay)
	{
		UGameplayStatics::PlaySoundAtLocation(Character, SoundToPlay, Character->GetActorLocation());
	}
}

void UGA_MeleeCombo::OnComboMontageEnded()
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (ACombat_Player* Player = Cast<ACombat_Player>(Character))
	{
		Player->SetAttacking(false);
		//restores movement after combo ends
		Player->GetCharacterMovement()->MaxWalkSpeed = StoredMaxWalkSpeed;
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}