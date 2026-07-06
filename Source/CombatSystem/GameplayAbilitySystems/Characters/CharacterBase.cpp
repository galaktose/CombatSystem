// Fill out your copyright notice in the Description page of Project Settings.
#include "CharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInput/EnhancedInputComponent.h"          // ADDED
#include "GameplayTagContainer.h"             // ADDED

ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(false);
	AbilitySystemComponent->SetReplicationMode(ReplicationMode);

	CombatAttributeSet = CreateDefaultSubobject<UCombatAttributeSet>(TEXT("CombatAttributeSet")); // ADDED

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

void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (IA_Attack)
			EIC->BindAction(IA_Attack, ETriggerEvent::Started, this, &ACharacterBase::Input_Attack);
		if (IA_Aim)
		{
			EIC->BindAction(IA_Aim, ETriggerEvent::Started, this, &ACharacterBase::Input_Aim, true);
			EIC->BindAction(IA_Aim, ETriggerEvent::Completed, this, &ACharacterBase::Input_Aim, false);
		}
		if (IA_SwitchStance)
			EIC->BindAction(IA_SwitchStance, ETriggerEvent::Started, this, &ACharacterBase::Input_ToggleStance);
		if (IA_Reload)
			EIC->BindAction(IA_Reload, ETriggerEvent::Started, this, &ACharacterBase::Input_Reload);
		if (IA_Special)
			EIC->BindAction(IA_Special, ETriggerEvent::Started, this, &ACharacterBase::Input_Special);
	}
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

// ADDED below
void ACharacterBase::SetStance(ECombatStance NewStance)
{
	CurrentStance = NewStance;

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

void ACharacterBase::Input_ToggleStance()
{
	ToggleStance();
}

void ACharacterBase::Input_Attack()
{
	// Day 3: activate melee combo or ranged fire ability depending on CurrentStance
}

void ACharacterBase::Input_Aim(bool bStarted)
{
	// Day 3: add/remove State.Aiming loose tag based on bStarted
}

void ACharacterBase::Input_Reload()
{
	// Day 3
}

void ACharacterBase::Input_Special()
{
	// Day 5
}