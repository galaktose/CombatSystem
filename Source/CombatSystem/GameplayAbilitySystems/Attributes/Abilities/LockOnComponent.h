// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"	
#include "Components/ActorComponent.h"
#include "LockOnComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLockOnTargetChangedSignature, AActor*, NewTarget);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class COMBATSYSTEM_API ULockOnComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULockOnComponent();

	UFUNCTION(BlueprintCallable, Category = "LockOn")
	void ToggleLockOn();

	UFUNCTION(BlueprintCallable, Category = "LockOn")
	void ClearLockOn();

	UFUNCTION(BlueprintCallable, Category = "LockOn")
	AActor* GetLockedTarget() const { return CurrentTarget.Get(); }

	UFUNCTION(BlueprintCallable, Category = "LockOn")
	bool IsLockedOn() const { return CurrentTarget.IsValid(); }

	UPROPERTY(BlueprintAssignable, Category = "LockOn|Events")
	FOnLockOnTargetChangedSignature OnLockOnTargetChanged; // for camera/UI to react to

protected:
	UPROPERTY(EditDefaultsOnly, Category = "LockOn")
	float LockOnRange = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "LockOn")
	float LockOnSearchRadius = 300.f;

	TWeakObjectPtr<AActor> CurrentTarget;

	AActor* FindBestTarget() const;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
