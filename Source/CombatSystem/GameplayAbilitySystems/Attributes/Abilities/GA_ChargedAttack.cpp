// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_ChargedAttack.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
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
		false, Ignore, EDrawDebugTrace::None, Hits, true);

	TSet<AActor*> AlreadyHit;
	bool bAnyHitLanded = false;

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
					// launch the target into the air
					TargetChar->LaunchCharacter(FVector(0.f, 0.f, LaunchVelocityZ), true, true);
					CachedTargetCharacter = TargetChar; 
				}
			}
		}
	}

	CachedPlayerCharacter = Character; // ADDED

	if (LaunchMontage)
	{
		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, NAME_None, LaunchMontage, 1.f, FName("ChargedAttack_Up_Start"));
		MontageTask->OnCompleted.AddDynamic(this, &UGA_ChargedAttack::OnLaunchMontageEnded);
		MontageTask->OnInterrupted.AddDynamic(this, &UGA_ChargedAttack::OnLaunchMontageEnded);
		MontageTask->ReadyForActivation();
	}

	if (bAnyHitLanded) // only schedule the freeze/follow-up if something was actually hit
	{
		UAbilityTask_WaitDelay* RiseDelay = UAbilityTask_WaitDelay::WaitDelay(this, RiseDuration);
		RiseDelay->OnFinish.AddDynamic(this, &UGA_ChargedAttack::OnRiseDelayFinished);
		RiseDelay->ReadyForActivation();
	}
	else if (!LaunchMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void UGA_ChargedAttack::OnRiseDelayFinished()
{
	if (!CachedTargetCharacter.IsValid()) return;

	ACharacter* TargetChar = CachedTargetCharacter.Get();

	// Freeze the target in place at wherever their launch arc has carried them to
	TargetChar->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	TargetChar->GetCharacterMovement()->Velocity = FVector::ZeroVector;
	TargetChar->GetCharacterMovement()->MaxFlySpeed = 0.f;

	FVector TargetFrozenLocation = TargetChar->GetActorLocation();

	if (CachedPlayerCharacter.IsValid())
	{
		ACharacter* PlayerChar = CachedPlayerCharacter.Get();

		if (ACharacterBase* PlayerBase = Cast<ACharacterBase>(PlayerChar))
		{
			PlayerBase->SetAirborne(true);
			PlayerBase->StartAirborneFallTimer(AirborneFallDelay);
		}

		PlayerChar->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		PlayerChar->GetCharacterMovement()->Velocity = FVector::ZeroVector;
		PlayerChar->GetCharacterMovement()->MaxFlySpeed = 0.f;

		FVector PlayerLoc = PlayerChar->GetActorLocation();
		FVector PlayerNewLocation = FVector(PlayerLoc.X, PlayerLoc.Y, TargetFrozenLocation.Z);
		PlayerChar->SetActorLocation(PlayerNewLocation, true); // player's own teleport, matching target's height
	}
}

void UGA_ChargedAttack::OnLaunchMontageEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}