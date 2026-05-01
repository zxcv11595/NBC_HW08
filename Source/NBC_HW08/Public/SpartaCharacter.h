// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpartaCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;
class UWidgetComponent;

UCLASS()
class NBC_HW08_API ASpartaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASpartaCharacter();


	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
	float Health;

	float SlowMultiplier;
	bool bIsSprinting;

	FTimerHandle SlowTimerHandle;
	FTimerHandle ReverseControlTimerHandle;

	bool bIsSlowed = false;
	bool bReverseControl = false;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,	// 스킬 등
		AController* EventInstigator,			// 데미지를 누가 입혔는지(캐릭터, 여기서는 nullptr)
		AActor* DamageCauser) override;			// 데미지를 입힌 오브젝트

	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()
	void StartJump(const FInputActionValue& value);
	UFUNCTION()
	void StopJump(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void StartSprint(const FInputActionValue& value);
	UFUNCTION()
	void StopSprint(const FInputActionValue& value);

	void OnDeath();
	void UpdateOverheadHP();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent*	SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent*		CameraComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent*		OverheadWidget;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHeatlh(float Amount);

	UFUNCTION(BlueprintPure, Category = "Debuff")
	FString GetDebuffStatusText() const;

	void UpdateMovementSpeed();

	void ApplySlow(float SlowDuration);
	void RemoveSlow();

	void ApplyReverseControl(float ReverseDuration);
	void RemoveReverseControl();

private:
	float NormalSpeed;
	float SprintSpeedMultiplier;
	float SprintSpeed;

};
