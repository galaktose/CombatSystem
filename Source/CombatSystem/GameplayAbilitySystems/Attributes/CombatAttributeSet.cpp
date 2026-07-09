// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "../Characters/CharacterBase.h"
#include "Net/UnrealNetwork.h"

UCombatAttributeSet::UCombatAttributeSet()
{
	Health.SetBaseValue(100.f);
	Health.SetCurrentValue(100.f);
	MaxHealth.SetBaseValue(100.f);
	MaxHealth.SetCurrentValue(100.f);
	CurrentAmmo.SetBaseValue(30.f);
	CurrentAmmo.SetCurrentValue(30.f);
	MaxAmmo.SetBaseValue(30.f);
	MaxAmmo.SetCurrentValue(30.f);
}

void UCombatAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float Damage = GetIncomingDamage();
		SetIncomingDamage(0.f);

		if (Damage > 0.f)
		{
			const float NewHealth = FMath::Clamp(GetHealth() - Damage, 0.f, GetMaxHealth());
			SetHealth(NewHealth);
		}
		if (ACharacterBase* CharTarget = Cast<ACharacterBase>(Data.Target.GetAvatarActor()))
		{
			CharTarget->OnHitReceived(Damage, false); // wire bWasCritical through later if needed
		}
	}
}

void UCombatAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCombatAttributeSet, Health);
	DOREPLIFETIME(UCombatAttributeSet, MaxHealth);
	DOREPLIFETIME(UCombatAttributeSet, CurrentAmmo);
	DOREPLIFETIME(UCombatAttributeSet, MaxAmmo);
}