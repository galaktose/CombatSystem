// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GA_AbilityBase.h"
#include "../CombatData.h"
#include "GA_DashAttack.generated.h"

UCLASS()
class COMBATSYSTEM_API UGA_DashAttack : public UGA_CombatAbilityBase
{
	GENERATED_BODY()

public:
	UGA_DashAttack();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly, Category = "Special")
	class UAnimMontage* DashMontage;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Special")
	FSpecialAbilityStats Stats;

	UPROPERTY(EditDefaultsOnly, Category = "Special")
	float DashDistance = 500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Special")
	float DashDuration = 0.2f;

	UFUNCTION()
	void OnDashStartMontageEnded();

	void PerformDashMovement();

	UFUNCTION()
	void OnDashMontageEnded();
};
