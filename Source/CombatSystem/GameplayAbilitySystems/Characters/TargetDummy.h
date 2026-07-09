// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "CharacterBase.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "TargetDummy.generated.h"


UCLASS()
class COMBATSYSTEM_API ATargetDummy : public ACharacterBase
{
	GENERATED_BODY()

public:
	ATargetDummy();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TArray<TSubclassOf<class UGameplayAbility>> AutoAttackAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float AttackInterval = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float RespawnRadius = 500.f;

	bool bIsDead = false; //Flag to track if the dummy is dead
	FTimerHandle AttackTimerHandle;
	FVector SpawnOrigin;

	void PerformAutoAttack();
	void HandleDeath();
	void Respawn();

	void OnHealthChanged(const FOnAttributeChangeData& Data);
};
