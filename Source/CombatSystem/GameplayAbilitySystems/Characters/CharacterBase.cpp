// Fill out your copyright notice in the Description page of Project Settings.
#include "CharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTagContainer.h"

ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(false);
	AbilitySystemComponent->SetReplicationMode(ReplicationMode);

	CombatAttributeSet = CreateDefaultSubobject<UCombatAttributeSet>(TEXT("CombatAttributeSet"));

	GetCapsuleComponent()->InitCapsuleSize(35.0f, 90.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.f;
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}

void ACharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}

UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ACharacterBase::SetStance(ECombatStance NewStance)
{
	CurrentStance = NewStance;
	UE_LOG(LogTemp, Warning, TEXT("Stance changed to: %s"), NewStance == ECombatStance::Melee ? TEXT("Melee") : TEXT("Ranged"));

	if (AbilitySystemComponent)
	{
		FGameplayTag MeleeTag = FGameplayTag::RequestGameplayTag(FName("Stance.Melee"));
		FGameplayTag RangedTag = FGameplayTag::RequestGameplayTag(FName("Stance.Ranged"));

		AbilitySystemComponent->RemoveLooseGameplayTag(MeleeTag);
		AbilitySystemComponent->RemoveLooseGameplayTag(RangedTag);

		AbilitySystemComponent->AddLooseGameplayTag(
			NewStance == ECombatStance::Melee ? MeleeTag : RangedTag);
	}
}

void ACharacterBase::ToggleStance()
{
	SetStance(CurrentStance == ECombatStance::Melee ? ECombatStance::Ranged : ECombatStance::Melee);
}