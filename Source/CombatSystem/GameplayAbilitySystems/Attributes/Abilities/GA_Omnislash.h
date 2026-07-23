// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA_AbilityBase.h"
#include "GA_OmniSlash.generated.h"

UCLASS()
class COMBATSYSTEM_API UGA_OmniSlash : public UGA_CombatAbilityBase
{
	GENERATED_BODY()

public:
	UGA_OmniSlash();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	class UAnimMontage* OmniSlashMontage; // can reuse a single dash/slash anim across all 8 hits

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float DamagePerHit = 8.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float FinalHitDamageMultiplier = 3.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float LockOnRange = 800.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float StrikeDistanceFromTarget = 100.f; // how close the teleport lands relative to target

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float TraceRadius = 80.f;

	int32 CurrentHitIndex = 0;
	int32 TotalHits = 8;

	TWeakObjectPtr<AActor> LockedTarget;

	UPROPERTY()
	class UAbilityTask_WaitGameplayEvent* WaitHitEventTask;

	UFUNCTION()
	void OnHitEventReceived(FGameplayEventData Payload);

	UFUNCTION()
	void OnOmniSlashMontageEnded();

	void FindLockOnTarget();
	void PerformStrike(bool bIsFinalHit);

	// Single, clearly-marked function where a future hit lands - this is where Stagger increment would plug in later
	void OnHitLanded(AActor* HitActor, float DamageDealt, bool bIsFinalHit);
};
