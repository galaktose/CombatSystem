// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CombatData.generated.h"

USTRUCT(BlueprintType)
struct FSpecialAbilityStats : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Special")
	float Damage = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Special")
	float Range = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Special")
	float CooldownDuration = 5.f;
};