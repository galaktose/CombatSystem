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

	// Damage and trace parameters for the melee combo
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float BaseDamage = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float TraceRadius = 60.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float TraceRange = 150.f;

	// Sound effects for combo hits and misses
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TArray<class USoundBase*> ComboHitSounds; // index 0 = Combo1 hit, 1 = Combo2 hit, 2 = Combo3 hit

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TArray<class USoundBase*> ComboMissSounds; 

	// Current montage task for the combo
	UPROPERTY()
	class UAbilityTask_PlayMontageAndWait* CurrentMontageTask;
	bool bComboActive = false;

	float StoredMaxWalkSpeed = 500.f;

	void DoMeleeTrace(bool bIsCritical, int32 ComboStageIndex);
};
