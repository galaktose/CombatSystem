// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Test.h"

void UGA_TestAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp, Warning, TEXT("GA_Test activated!"));
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}