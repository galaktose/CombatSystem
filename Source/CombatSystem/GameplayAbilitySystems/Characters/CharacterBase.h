// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "../Attributes/CombatAttributeSet.h"       
#include "CharacterBase.generated.h"

UENUM(BlueprintType)                  
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

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")   
		class UCombatAttributeSet* CombatAttributeSet;                             

	UPROPERTY(BlueprintReadOnly, Category = "Combat")                          
		ECombatStance CurrentStance = ECombatStance::Melee;                       

	UFUNCTION(BlueprintCallable, Category = "Combat")                          
		void SetStance(ECombatStance NewStance);                                  

	UFUNCTION(BlueprintCallable, Category = "Combat")                          
		void ToggleStance();                                           
		
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnHitReceived(float DamageAmount, bool bWasCritical);

	// Airborne state handling
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bIsAirborne = false;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetAirborne(bool bAirborne);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnAirborneFall(); // for landing animation or sfx

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StartAirborneFallTimer(float Duration);

	virtual void HandleAirborneFall();
	virtual void InterruptAirborneAction();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilitySystem")
	EGameplayEffectReplicationMode ReplicationMode = EGameplayEffectReplicationMode::Mixed;

	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	virtual void Tick(float DeltaTime) override;

	// Airborne fall handling
	FTimerHandle AirborneFallTimer;
	

};