// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA_AbilityBase.h"
#include "GA_AirCombo.generated.h"

/**
 * 
 */
UCLASS()
class COMBATSYSTEM_API UGA_AirCombo : public UGA_CombatAbilityBase
{
	GENERATED_BODY()
public:
	UGA_AirCombo();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	class UAnimMontage* AirComboMontage;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float BaseDamage = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float TraceRadius = 70.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float TraceRange = 150.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float SlamDamageMultiplier = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float AirborneFallResetDuration = 3.f; // matches your existing AirborneFallDelay

	UPROPERTY()
	class UAbilityTask_WaitGameplayEvent* WaitSlamTriggerTask;

	UFUNCTION()
	void OnSlamTriggerReceived(FGameplayEventData Payload);

	// cache whats needed to perform the slam once the notify fires
	TWeakObjectPtr<AActor> CachedSlamTarget;

	void DoAirTrace(bool bIsSlam);

	UFUNCTION()
	void OnAirComboMontageEnded();
};
