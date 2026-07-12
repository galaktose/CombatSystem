// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_Melee_Hit.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTagContainer.h"

void UAnimNotify_Melee_Hit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (AActor* Owner = MeshComp->GetOwner())
	{
		FGameplayEventData EventData;
		FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Event.Melee.HitWindow"));
		EventData.EventTag = EventTag;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, EventTag, EventData);
	}
}
