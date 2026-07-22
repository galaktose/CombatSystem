// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "AnimNotify_Melee_Hit.generated.h"

/**
 * 
 */
UCLASS()
class COMBATSYSTEM_API UAnimNotify_Melee_Hit : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	UPROPERTY(EditAnywhere, Category = "GameplayEvent")
	FGameplayTag EventTagToSend;
};
