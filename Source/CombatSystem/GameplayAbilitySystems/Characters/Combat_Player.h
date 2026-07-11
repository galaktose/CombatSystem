// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "Combat_Player.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, float, NewHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChangedSignature, float, NewAmmo, float, MaxAmmo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnComboChangedSignature, int32, NewComboCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpecialStatusChangedSignature, FText, NewStatus);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCriticalHitSignature);

UCLASS()
class COMBATSYSTEM_API ACombat_Player : public ACharacterBase
{
	GENERATED_BODY()

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;

	// Blueprint events for UI or other systems to bind to
	UPROPERTY(BlueprintAssignable, Category = "Combat|Events")
	FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Combat|Events")
	FOnAmmoChangedSignature OnAmmoChanged;

	UPROPERTY(BlueprintAssignable, Category = "Combat|Events")
	FOnComboChangedSignature OnComboChanged;

	UPROPERTY(BlueprintAssignable, Category = "Combat|Events")
	FOnSpecialStatusChangedSignature OnSpecialStatusChanged;

	UPROPERTY(BlueprintAssignable, Category = "Combat|Events")
	FOnCriticalHitSignature OnCriticalHitLanded;
	// Function to get the current status of the special ability
	UFUNCTION(BlueprintCallable, Category = "Combat")
	FText GetSpecialAbilityStatus() const;
	// Blueprint events for playing animations
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void PlayShootAnim();

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void PlayReloadAnim();
	// Last hit target for combo system
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	AActor* LastHitTarget = nullptr;

protected:
	void Input_Attack();
	void Input_Aim(bool bStarted);
	void Input_ToggleStance();
	void Input_Reload();
	void Input_Special();
	void HandleHealthAttributeChanged(const FOnAttributeChangeData& Data);
	void HandleAmmoAttributeChanged(const FOnAttributeChangeData& Data);
	void HandleCooldownTagChanged(const FGameplayTag Tag, int32 NewCount);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnAimStateChanged(bool bIsAiming);

	// Input Actions
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Attack;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Aim;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_SwitchStance;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Reload;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Special;

	//Ability Classes
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<class UGameplayAbility> TestAbilityClass;
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<class UGameplayAbility> MeleeComboAbilityClass;
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<class UGameplayAbility> RangedFireAbilityClass; 

	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<class UGameplayAbility> SpecialAbilityClass;

	// Stance change event
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnStanceChanged(ECombatStance NewStance);

	// Combo system
	int32 MeleeComboCount = 0;
	FTimerHandle ComboResetTimerHandle;
	void ResetCombo();
};