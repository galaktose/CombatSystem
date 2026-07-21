// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_ChargedAttack.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "../../Characters/Combat_Player.h"

UGA_ChargedAttack::UGA_ChargedAttack()
{
	ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Stance.Melee")));
}

void UGA_ChargedAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Face camera direction, same as melee combo
	if (APlayerController* PC = Cast<APlayerController>(Character->GetController()))
	{
		FRotator CamRotation = PC->GetControlRotation();
		Character->SetActorRotation(FRotator(0.f, CamRotation.Yaw, 0.f));
	}

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
	bool bAnyHitLanded = false;

	FVector TargetFinalLocation = FVector::ZeroVector; // hold the target's actual z position
	bool bHasTargetLocation = false;

	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && !AlreadyHit.Contains(HitActor))
		{
			AlreadyHit.Add(HitActor);
			bAnyHitLanded = true;

			ApplyDamageToTarget(HitActor, Damage);

			if (ACharacterBase* TargetBase = Cast<ACharacterBase>(HitActor))
			{
				TargetBase->SetAirborne(true);
				TargetBase->StartAirborneFallTimer(AirborneFallDelay);

				if (ACharacter* TargetChar = Cast<ACharacter>(HitActor))
				{
					TargetChar->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
					TargetChar->GetCharacterMovement()->Velocity = FVector::ZeroVector;
					TargetChar->GetCharacterMovement()->MaxFlySpeed = 0.f;

					FVector TargetNewLocation = TargetChar->GetActorLocation() + FVector(0.f, 0.f, PopUpHeight);
					TargetChar->SetActorLocation(TargetNewLocation, true);

					TargetFinalLocation = TargetNewLocation; // capture where the target actually ended up
					bHasTargetLocation = true; // ADDED
				}
			}
		}
	}

	if (bAnyHitLanded)
	{
		if (ACharacterBase* PlayerBase = Cast<ACharacterBase>(Character))
		{
			PlayerBase->SetAirborne(true);
			PlayerBase->StartAirborneFallTimer(AirborneFallDelay);
		}

		Character->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		Character->GetCharacterMovement()->Velocity = FVector::ZeroVector;
		Character->GetCharacterMovement()->MaxFlySpeed = 0.f;

		// MODIFIED — sync player's Z to the target's actual final height, not an independent offset
		if (bHasTargetLocation)
		{
			FVector PlayerLocation = Character->GetActorLocation();
			FVector PlayerNewLocation = FVector(PlayerLocation.X, PlayerLocation.Y, TargetFinalLocation.Z);
			Character->SetActorLocation(PlayerNewLocation, true);
		}
		else
		{
			// fallback, shouldn't happen since this block only runs when bAnyHitLanded is true
			FVector PlayerNewLocation = Character->GetActorLocation() + FVector(0.f, 0.f, PopUpHeight);
			Character->SetActorLocation(PlayerNewLocation, true);
		}
	}
}

void UGA_ChargedAttack::OnLaunchMontageEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}