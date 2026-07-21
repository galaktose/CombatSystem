// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA_AbilityBase.h"
#include "GA_ChargedAttack.generated.h"

/**
 * 
 */
UCLASS()
class COMBATSYSTEM_API UGA_ChargedAttack : public UGA_CombatAbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_ChargedAttack();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	class UAnimMontage* LaunchMontage;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float Damage = 15.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float TraceRadius = 60.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float TraceRange = 150.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float PopUpHeight = 300.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float AirborneFallDelay = 3.f;

	UFUNCTION()
	void OnLaunchMontageEnded();

};
