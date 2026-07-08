// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_AbilityBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

void UGA_CombatAbilityBase::ApplyDamageToTarget(AActor* TargetActor, float DamageAmount)
{
	if (!TargetActor || !DamageEffectClass) 
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyDamageToTarget bailed early — TargetActor: %s, DamageEffectClass: %s"),
			TargetActor ? TEXT("valid") : TEXT("NULL"),
			DamageEffectClass ? TEXT("valid") : TEXT("NULL"));
		return;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!TargetASC) 
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyDamageToTarget: Target %s has no ASC"), *TargetActor->GetName());
		return;
	}

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (!SourceASC) return;

	FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
	ContextHandle.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, 1.f, ContextHandle);
	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), DamageAmount);
		SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
		UE_LOG(LogTemp, Warning, TEXT("Applied %.1f damage to %s"), DamageAmount, *TargetActor->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SpecHandle invalid, GE not applied"));
	}

}
