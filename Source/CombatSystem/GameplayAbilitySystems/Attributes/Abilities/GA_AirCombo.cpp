// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_AirCombo.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "../../Characters/Combat_Player.h"

UGA_AirCombo::UGA_AirCombo()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Airborne")));
}

void UGA_AirCombo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!TriggerEventData)
	{
		UE_LOG(LogTemp, Error, TEXT("TriggerEventData is null"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Air Combo Event: %s"),
		*TriggerEventData->EventTag.ToString());
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());

	bool bIsSlam = TriggerEventData &&
		TriggerEventData->EventTag == FGameplayTag::RequestGameplayTag(FName("Ability.Air.Combo4"));

	FName SectionName = NAME_None;
	if (TriggerEventData->EventTag == FGameplayTag::RequestGameplayTag(FName("Ability.Air.Combo1")))
		SectionName = FName("AirCombo1");
	else if (TriggerEventData->EventTag == FGameplayTag::RequestGameplayTag(FName("Ability.Air.Combo2")))
		SectionName = FName("AirCombo2");
	else if (TriggerEventData->EventTag == FGameplayTag::RequestGameplayTag(FName("Ability.Air.Combo3")))
		SectionName = FName("AirCombo3");
	else if (TriggerEventData->EventTag == FGameplayTag::RequestGameplayTag(FName("Ability.Air.Combo4")))
		SectionName = FName("AirCombo4_Slam");

	DoAirTrace(bIsSlam);

	if (AirComboMontage && SectionName != NAME_None)
	{
		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, NAME_None, AirComboMontage, 1.f, SectionName);
		MontageTask->OnCompleted.AddDynamic(this, &UGA_AirCombo::OnAirComboMontageEnded);
		MontageTask->OnInterrupted.AddDynamic(this, &UGA_AirCombo::OnAirComboMontageEnded);
		MontageTask->OnCancelled.AddDynamic(this, &UGA_AirCombo::OnAirComboMontageEnded);
		MontageTask->ReadyForActivation();
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void UGA_AirCombo::DoAirTrace(bool bIsSlam)
{
	UE_LOG(LogTemp, Warning, TEXT("DoAirTrace called"));
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character) return;

	FVector Start = Character->GetActorLocation() + Character->GetActorForwardVector() * 50.f;
	FVector End = Start + Character->GetActorForwardVector() * TraceRange;

	TArray<FHitResult> Hits;
	TArray<AActor*> Ignore;
	Ignore.Add(Character);

	UKismetSystemLibrary::SphereTraceMulti(
		Character, Start, End, TraceRadius,
		UEngineTypes::ConvertToTraceType(ECC_Pawn),
		false, Ignore, EDrawDebugTrace::ForDuration, Hits, true);

	TSet<AActor*> AlreadyHit;
	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor(); 
		if (HitActor && !AlreadyHit.Contains(HitActor))
		{
			AlreadyHit.Add(HitActor);

			float Damage = bIsSlam ? BaseDamage * SlamDamageMultiplier : BaseDamage;
			ApplyDamageToTarget(HitActor, Damage);

			if (ACharacterBase* TargetBase = Cast<ACharacterBase>(HitActor)) 
			{
				if (bIsSlam)
				{
					TargetBase->HandleAirborneFall();
				}
				else
				{
					TargetBase->StartAirborneFallTimer(AirborneFallResetDuration);
				}
			}
		}
	} 

	// Player-side slam handling
	if (bIsSlam)
	{
		if (ACombat_Player* Player = Cast<ACombat_Player>(Character))
		{
			Player->HandleAirborneFall();
		}
	}
}

void UGA_AirCombo::OnAirComboMontageEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
