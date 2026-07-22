// Fill out your copyright notice in the Description page of Project Settings.
#include "Combat_Player.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

void ACombat_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (IA_Attack_Tap)
			EIC->BindAction(IA_Attack_Tap, ETriggerEvent::Triggered, this, &ACombat_Player::Input_Attack_Tap);
		if (IA_Attack_Hold)
			EIC->BindAction(IA_Attack_Hold, ETriggerEvent::Triggered, this, &ACombat_Player::Input_Attack_Hold);
		EIC->BindAction(IA_Attack_Hold, ETriggerEvent::Completed, this, &ACombat_Player::Input_Attack_Hold_Released);
		if (IA_Aim)
		{
			EIC->BindAction(IA_Aim, ETriggerEvent::Started, this, &ACombat_Player::Input_Aim, true);
			EIC->BindAction(IA_Aim, ETriggerEvent::Completed, this, &ACombat_Player::Input_Aim, false);
		}
		if (IA_SwitchStance)
			EIC->BindAction(IA_SwitchStance, ETriggerEvent::Started, this, &ACombat_Player::Input_ToggleStance);
		if (IA_Reload)
			EIC->BindAction(IA_Reload, ETriggerEvent::Started, this, &ACombat_Player::Input_Reload);
		if (IA_Special)
			EIC->BindAction(IA_Special, ETriggerEvent::Started, this, &ACombat_Player::Input_Special);
		if (IA_Special2)
			EIC->BindAction(IA_Special2, ETriggerEvent::Started, this, &ACombat_Player::Input_OmniSlash);
	}
}



void ACombat_Player::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	SetStance(CurrentStance);

	if (!AbilitySystemComponent) return;

	if (MeleeComboAbilityClass)
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(MeleeComboAbilityClass, 1));
	}

	if (RangedFireAbilityClass)
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(RangedFireAbilityClass, 1));
	}

	if (SpecialAbilityClass)
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(SpecialAbilityClass, 1));
	}
	if (ChargedAttackAbilityClass)
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(ChargedAttackAbilityClass, 1));
	}
	// Register attribute change 
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		UCombatAttributeSet::GetHealthAttribute()).AddUObject(this, &ACombat_Player::HandleHealthAttributeChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		UCombatAttributeSet::GetCurrentAmmoAttribute()).AddUObject(this, &ACombat_Player::HandleAmmoAttributeChanged);

	FGameplayTag CooldownTag = FGameplayTag::RequestGameplayTag(FName("State.Cooldown.Special"));
	AbilitySystemComponent->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this, &ACombat_Player::HandleCooldownTagChanged);

	// test case
	/*if (TestAbilityClass)
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(TestAbilityClass, 1));
	}*/
}


void ACombat_Player::Input_ToggleStance()
{
	const FGameplayTag AimTag = FGameplayTag::RequestGameplayTag(FName("State.Aiming"));
	if (AbilitySystemComponent && AbilitySystemComponent->HasMatchingGameplayTag(AimTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot switch stance while aiming"));
		return;
	}
	ToggleStance(); //Inherited from CharacterBase
	OnStanceChanged(CurrentStance);
	OnSpecialStatusChanged.Broadcast(GetSpecialAbilityStatus());
}

void ACombat_Player::Input_Attack_Tap()
{
	/*if (bIsAirborne)
	{
		if (AbilitySystemComponent && AirComboAbilityClass)
		{
			AbilitySystemComponent->TryActivateAbilityByClass(AirComboAbilityClass);
		}
		return;
	}*/

	if (CurrentStance == ECombatStance::Melee)
	{
		float CurrentTime = GetWorld()->GetTimeSeconds();
		if (CurrentTime - LastAttackInputTime < AttackInputCooldown)
		{
			return;
		}
		LastAttackInputTime = CurrentTime;

		MeleeComboCount = (MeleeComboCount % 3) + 1;
		OnComboChanged.Broadcast(MeleeComboCount);

		// cancel any currently running melee combo ability before starting the next attack in the sequence
		if (AbilitySystemComponent && MeleeComboAbilityClass)
		{
			FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromClass(MeleeComboAbilityClass);
			if (Spec && Spec->IsActive())
			{
				AbilitySystemComponent->CancelAbilityHandle(Spec->Handle);
			}
		}

		FGameplayTag ComboTag = FGameplayTag::RequestGameplayTag(
			FName(*FString::Printf(TEXT("Ability.Melee.Combo%d"), MeleeComboCount)));

		FGameplayEventData EventData;
		EventData.EventTag = ComboTag;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, ComboTag, EventData);

		GetWorldTimerManager().ClearTimer(ComboResetTimerHandle);
		GetWorldTimerManager().SetTimer(ComboResetTimerHandle, this, &ACombat_Player::ResetCombo, 3.0f, false);
	}
	else if (CurrentStance == ECombatStance::Ranged)
	{
		const FGameplayTag AimTag = FGameplayTag::RequestGameplayTag(FName("State.Aiming"));
		const bool bHasAimTag = AbilitySystemComponent->HasMatchingGameplayTag(AimTag);

		//UE_LOG(LogTemp, Warning, TEXT("Ranged attack attempted, HasAimTag: %s"),bHasAimTag ? TEXT("true") : TEXT("false"));

		if (bHasAimTag &&
			CombatAttributeSet &&
			CombatAttributeSet->GetCurrentAmmo() > 0)
		{
			AbilitySystemComponent->TryActivateAbilityByClass(RangedFireAbilityClass);
			CombatAttributeSet->SetCurrentAmmo(CombatAttributeSet->GetCurrentAmmo() - 1.f);
		}
	}
}

void ACombat_Player::Input_Attack_Hold()
{
	if (bChargedAttackTriggered) return; // charge attack once per hold
	bChargedAttackTriggered = true;

	if (CurrentStance == ECombatStance::Melee && !bIsAirborne && AbilitySystemComponent && ChargedAttackAbilityClass)
	{
		AbilitySystemComponent->TryActivateAbilityByClass(ChargedAttackAbilityClass);
	}
}

void ACombat_Player::ResetCombo()
{
	MeleeComboCount = 0;
	OnComboChanged.Broadcast(MeleeComboCount);
}

void ACombat_Player::Input_Attack_Hold_Released()
{
	bChargedAttackTriggered = false; // reset for next hold
}


void ACombat_Player::Input_Aim(bool bStarted)
{
	if (CurrentStance != ECombatStance::Ranged)
	{
		return; // aiming only relevant in Ranged stance
	}

	UE_LOG(LogTemp, Warning, TEXT("Input_Aim called, bStarted: %s"), bStarted ? TEXT("true") : TEXT("false"));

	FGameplayTag AimTag = FGameplayTag::RequestGameplayTag(FName("State.Aiming"));
	if (bStarted) 
	{
		AbilitySystemComponent->AddLooseGameplayTag(AimTag);
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	else 
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(AimTag);
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
	OnAimStateChanged(bStarted);
}


void ACombat_Player::Input_Reload()
{
	// block reload outside Ranged stance
	if (CurrentStance != ECombatStance::Ranged)
	{
		return;
	}

	// block reload if already reloading
	if (bIsReloading)
	{
		return;
	}

	// block reload if already at max ammo
	if (CombatAttributeSet && CombatAttributeSet->GetCurrentAmmo() >= CombatAttributeSet->GetMaxAmmo())
	{
		return;
	}

	bIsReloading = true; 

	PlayReloadAnim();
}

void ACombat_Player::OnReloadAnimComplete()
{
	if (CombatAttributeSet)
	{
		CombatAttributeSet->SetCurrentAmmo(CombatAttributeSet->GetMaxAmmo());
	}
	bIsReloading = false; 
}

void ACombat_Player::Input_Special()
{
	UE_LOG(LogTemp, Warning, TEXT("Input_Special called"));

	if (AbilitySystemComponent && SpecialAbilityClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempting to activate special ability"));
		bool bActivated = AbilitySystemComponent->TryActivateAbilityByClass(SpecialAbilityClass);
		UE_LOG(LogTemp, Warning, TEXT("TryActivateAbilityByClass result: %s"), bActivated ? TEXT("SUCCESS") : TEXT("FAILED"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Input_Special bailed - ASC: %s, SpecialAbilityClass: %s"),
			AbilitySystemComponent ? TEXT("valid") : TEXT("NULL"),
			SpecialAbilityClass ? TEXT("valid") : TEXT("NULL"));
	}
	FGameplayTag CDTag = FGameplayTag::RequestGameplayTag(FName("State.Cooldown.Special"));
	UE_LOG(LogTemp, Warning, TEXT("Post-activate, has cooldown tag: %s"),
		AbilitySystemComponent->HasMatchingGameplayTag(CDTag) ? TEXT("YES") : TEXT("NO"));
}

void ACombat_Player::Input_OmniSlash()
{
	UE_LOG(LogTemp, Warning, TEXT("Input_OmniSlash triggered"));
}

void ACombat_Player::HandleHealthAttributeChanged(const FOnAttributeChangeData& Data)
{
	if (CombatAttributeSet)
	{
		OnHealthChanged.Broadcast(Data.NewValue, CombatAttributeSet->GetMaxHealth());
	}
	if (!bIsDead && Data.NewValue <= 0.f)
	{
		bIsDead = true;
		HandleDeath();
	}
}


void ACombat_Player::HandleAmmoAttributeChanged(const FOnAttributeChangeData& Data)
{
	if (CombatAttributeSet)
	{
		OnAmmoChanged.Broadcast(Data.NewValue, CombatAttributeSet->GetMaxAmmo());
	}
}


void ACombat_Player::HandleCooldownTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	OnSpecialStatusChanged.Broadcast(GetSpecialAbilityStatus());
}


FText ACombat_Player::GetSpecialAbilityStatus() const
{
	if (!AbilitySystemComponent) return FText::FromString("Unknown");

	FGameplayTag CooldownTag = FGameplayTag::RequestGameplayTag(FName("State.Cooldown.Special"));
	if (AbilitySystemComponent->HasMatchingGameplayTag(CooldownTag))
	{
		return FText::FromString("On Cooldown");
	}

	if (CurrentStance != ECombatStance::Melee)
	{
		return FText::FromString("Disabled");
	}

	return FText::FromString("Active");
}


void ACombat_Player::HandleDeath()
{
	// Disable input during death sequence
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		DisableInput(PC);
	}

	OnDeath(); // BP plays death animation, ragdoll, fade, etc

	FTimerHandle RespawnTimer;
	GetWorldTimerManager().SetTimer(RespawnTimer, this, &ACombat_Player::HandlePlayerRespawn, 2.5f, false);
}

void ACombat_Player::HandlePlayerRespawn()
{
	SetActorLocation(PlayerSpawnOrigin);

	if (CombatAttributeSet)
	{
		CombatAttributeSet->SetHealth(CombatAttributeSet->GetMaxHealth());
	}

	bIsDead = false;

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		EnableInput(PC);
	}

	OnRespawned(); // BP can fade camera back in, etc
}