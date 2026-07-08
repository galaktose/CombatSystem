// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_AbilityBase.generated.h"

UCLASS()
class COMBATSYSTEM_API UGA_CombatAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<class UGameplayEffect> DamageEffectClass;

	// Applies DamageEffectClass to TargetActor, setting Data.Damage via SetByCaller
	void ApplyDamageToTarget(AActor* TargetActor, float DamageAmount);
};
