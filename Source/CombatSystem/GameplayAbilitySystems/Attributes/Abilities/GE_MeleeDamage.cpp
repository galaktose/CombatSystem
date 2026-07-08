// Fill out your copyright notice in the Description page of Project Settings.


#include "GE_MeleeDamage.h"
#include "../CombatAttributeSet.h"

UGE_MeleeDamage::UGE_MeleeDamage()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo ModifierInfo;
	ModifierInfo.Attribute = UCombatAttributeSet::GetIncomingDamageAttribute();
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;

	FSetByCallerFloat SetByCallerMagnitude;
	SetByCallerMagnitude.DataTag = FGameplayTag::RequestGameplayTag(FName("Data.Damage"));
	ModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCallerMagnitude);

	Modifiers.Add(ModifierInfo);
}
