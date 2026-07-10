// Fill out your copyright notice in the Description page of Project Settings.
#include "Combat_Player.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

void ACombat_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (IA_Attack)
			EIC->BindAction(IA_Attack, ETriggerEvent::Started, this, &ACombat_Player::Input_Attack);
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

	// test case
	/*if (TestAbilityClass)
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(TestAbilityClass, 1));
	}*/
}

void ACombat_Player::Input_ToggleStance()
{
	ToggleStance(); // inherited from ACharacterBase
}

void ACombat_Player::Input_Attack()
{
	//test case
	/*if (AbilitySystemComponent && TestAbilityClass)
	{
		AbilitySystemComponent->TryActivateAbilityByClass(TestAbilityClass);
	}*/
	UE_LOG(LogTemp, Warning, TEXT("Input_Attack called"));
	if (CurrentStance == ECombatStance::Melee)
	{
		MeleeComboCount = (MeleeComboCount % 3) + 1; // cycles 1,2,3

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

		UE_LOG(LogTemp, Warning, TEXT("Ranged attack attempted, HasAimTag: %s"),
			bHasAimTag ? TEXT("true") : TEXT("false"));

		if (bHasAimTag &&
			CombatAttributeSet &&
			CombatAttributeSet->GetCurrentAmmo() > 0)
		{
			AbilitySystemComponent->TryActivateAbilityByClass(RangedFireAbilityClass);
			CombatAttributeSet->SetCurrentAmmo(CombatAttributeSet->GetCurrentAmmo() - 1.f);
		}
	}

}

void ACombat_Player::ResetCombo()
{
	MeleeComboCount = 0;
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
		AbilitySystemComponent->AddLooseGameplayTag(AimTag);
	else
		AbilitySystemComponent->RemoveLooseGameplayTag(AimTag);

	OnAimStateChanged(bStarted);
}

void ACombat_Player::Input_Reload()
{
	if (CombatAttributeSet)
	{
		CombatAttributeSet->SetCurrentAmmo(CombatAttributeSet->GetMaxAmmo());
		PlayReloadAnim();
	}
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