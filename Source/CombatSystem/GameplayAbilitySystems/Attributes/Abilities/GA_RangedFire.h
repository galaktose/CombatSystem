// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA_AbilityBase.h"
#include "GA_RangedFire.generated.h"

/**
 * 
 */
UCLASS()
class COMBATSYSTEM_API UGA_RangedFire : public UGA_CombatAbilityBase
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	UGA_RangedFire();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float BaseDamage = 15.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float TraceRange = 3000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	class UAnimMontage* FireMontage;
	
};
