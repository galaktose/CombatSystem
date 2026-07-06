// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CombatAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class COMBATSYSTEM_API UCombatAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

	public:
		UCombatAttributeSet();

		UPROPERTY(BlueprintReadOnly, Category = "Health")
		FGameplayAttributeData Health;
		ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Health)

			UPROPERTY(BlueprintReadOnly, Category = "Health")
		FGameplayAttributeData MaxHealth;
		ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MaxHealth)

			UPROPERTY(BlueprintReadOnly, Category = "Ammo")
		FGameplayAttributeData CurrentAmmo;
		ATTRIBUTE_ACCESSORS(UCombatAttributeSet, CurrentAmmo)

			UPROPERTY(BlueprintReadOnly, Category = "Ammo")
		FGameplayAttributeData MaxAmmo;
		ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MaxAmmo)

			// Meta attribute - channel for incoming damage, not replicated/persisted directly
			UPROPERTY(BlueprintReadOnly, Category = "Damage")
		FGameplayAttributeData IncomingDamage;
		ATTRIBUTE_ACCESSORS(UCombatAttributeSet, IncomingDamage)

		virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
		virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
