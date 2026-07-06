// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "../Attributes/CombatAttributeSet.h"       // Added
#include "CharacterBase.generated.h"

UENUM(BlueprintType)                  // ADDED
enum class ECombatStance : uint8
{
	Melee UMETA(DisplayName = "Melee"),
	Ranged UMETA(DisplayName = "Ranged")
};

UCLASS()
class COMBATSYSTEM_API ACharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	ACharacterBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	class UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")   // ADDED
		class UCombatAttributeSet* CombatAttributeSet;                             // ADDED

	UPROPERTY(BlueprintReadOnly, Category = "Combat")                          // ADDED
		ECombatStance CurrentStance = ECombatStance::Melee;                       // ADDED

	UFUNCTION(BlueprintCallable, Category = "Combat")                          // ADDED
		void SetStance(ECombatStance NewStance);                                  // ADDED

	UFUNCTION(BlueprintCallable, Category = "Combat")                          // ADDED
		void ToggleStance();                                                       // ADDED

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilitySystem")
	EGameplayEffectReplicationMode ReplicationMode = EGameplayEffectReplicationMode::Mixed;

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:                                                                     // ADDED block
	void Input_Attack();
	void Input_Aim(bool bStarted);
	void Input_ToggleStance();
	void Input_Reload();
	void Input_Special();

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
};