// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "Combat_Player.generated.h"

UCLASS()
class COMBATSYSTEM_API ACombat_Player : public ACharacterBase
{
	GENERATED_BODY()

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;

protected:
	void Input_Attack();
	void Input_Aim(bool bStarted);
	void Input_ToggleStance();
	void Input_Reload();
	void Input_Special();

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Attack;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Aim;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_SwitchStance;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Reload;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Special;

	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<class UGameplayAbility> TestAbilityClass;
};