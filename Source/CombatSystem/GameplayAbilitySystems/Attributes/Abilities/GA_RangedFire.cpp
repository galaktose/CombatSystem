// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_RangedFire.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Combat_Player.h"
#include "../CombatAttributeSet.h"

UGA_RangedFire::UGA_RangedFire()
{
	ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Aiming")));
}

void UGA_RangedFire::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	APlayerController* PC = Cast<APlayerController>(Character->GetController());
	if (!PC)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	FVector CamLocation;
	FRotator CamRotation;
	PC->GetPlayerViewPoint(CamLocation, CamRotation); // matches true camera, so crosshair lines up

	FVector Start = CamLocation;
	FVector End = Start + CamRotation.Vector() * TraceRange;

	FHitResult Hit;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Character);

	bool bHit = UKismetSystemLibrary::LineTraceSingle(
		Character, Start, End, UEngineTypes::ConvertToTraceType(ECC_Pawn),
		false, ActorsToIgnore, EDrawDebugTrace::ForDuration, Hit, true);

	if (ACombat_Player* Player = Cast<ACombat_Player>(Character))
	{
		Player->PlayShootAnim();
	}

	if (bHit && Hit.GetActor())
	{
		ApplyDamageToTarget(Hit.GetActor(), BaseDamage);
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
