// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Abilities/GA_AbilityBase.h"
#include "GA_Dummy_AOESweep.generated.h"

/**
 * 
 */
UCLASS()
class COMBATSYSTEM_API UGA_DummyAOESweep : public UGA_CombatAbilityBase
{
	GENERATED_BODY()
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	class UAnimMontage* AttackMontage;

protected:
	UPROPERTY(EditDefaultsOnly) float Damage = 10.f;
	UPROPERTY(EditDefaultsOnly) float Radius = 300.f;

	UPROPERTY()
	class UAbilityTask_WaitGameplayEvent* WaitHitWindowTask;

	UFUNCTION()
	void OnHitWindowReceived(FGameplayEventData Payload);

	UFUNCTION()
	void OnAttackMontageEnded();
};
