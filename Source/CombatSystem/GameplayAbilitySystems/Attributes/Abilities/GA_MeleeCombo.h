// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GA_AbilityBase.h"
#include "GA_MeleeCombo.generated.h"

UCLASS()
class COMBATSYSTEM_API UGA_MeleeCombo : public UGA_CombatAbilityBase
{
	GENERATED_BODY()

public:
	UGA_MeleeCombo();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	class UAnimMontage* ComboMontage;

protected:
	UFUNCTION()
	void OnComboMontageEnded();

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float BaseDamage = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float TraceRadius = 60.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float TraceRange = 150.f;

	void DoMeleeTrace(bool bIsCritical);
};
