// Fill out your copyright notice in the Description page of Project Settings.
#include "Combat_Player.h"
#include "EnhancedInputComponent.h"

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

void ACombat_Player::Input_ToggleStance()
{
	ToggleStance(); // inherited from ACharacterBase
}

void ACombat_Player::Input_Attack()
{
	//Attack
}

void ACombat_Player::Input_Aim(bool bStarted)
{
	// Aiming
}

void ACombat_Player::Input_Reload()
{
	// Reload weapon
}

void ACombat_Player::Input_Special()
{
	// Special ability
}