// Fill out your copyright notice in the Description page of Project Settings.

#include "TargetDummy.h"
#include "AbilitySystemComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "../Attributes/CombatAttributeSet.h"

ATargetDummy::ATargetDummy()
{
}

void ATargetDummy::BeginPlay()
{
	Super::BeginPlay();

	SpawnOrigin = GetActorLocation();

	// init asc here
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		for (TSubclassOf<UGameplayAbility> AbilityClass : AutoAttackAbilities)
		{
			if (AbilityClass)
			{
				AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1));
			}
		}

		// Bind health change to detect death
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			UCombatAttributeSet::GetHealthAttribute()).AddUObject(this, &ATargetDummy::OnHealthChanged);
	}

	GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &ATargetDummy::PerformAutoAttack, AttackInterval, true);
}

void ATargetDummy::PerformAutoAttack()
{
	if (AutoAttackAbilities.Num() == 0 || !AbilitySystemComponent) return;

	int32 RandomIndex = FMath::RandRange(0, AutoAttackAbilities.Num() - 1);
	AbilitySystemComponent->TryActivateAbilityByClass(AutoAttackAbilities[RandomIndex]);
}

void ATargetDummy::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (Data.NewValue <= 0.f)
	{
		HandleDeath();
	}
}

void ATargetDummy::HandleDeath()
{
	// TODO: play death anim/VFX/SFX before respawn 
	GetWorldTimerManager().ClearTimer(AttackTimerHandle);

	FTimerHandle RespawnTimer;
	GetWorldTimerManager().SetTimer(RespawnTimer, this, &ATargetDummy::Respawn, 1.5f, false);
}

void ATargetDummy::Respawn()
{
	FVector RandomOffset = UKismetMathLibrary::RandomPointInBoundingBox(SpawnOrigin, FVector(RespawnRadius, RespawnRadius, 0.f));
	SetActorLocation(RandomOffset);

	if (CombatAttributeSet)
	{
		CombatAttributeSet->SetHealth(CombatAttributeSet->GetMaxHealth());
	}

	GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &ATargetDummy::PerformAutoAttack, AttackInterval, true);
}

