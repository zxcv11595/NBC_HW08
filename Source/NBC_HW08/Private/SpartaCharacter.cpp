// Fill out your copyright notice in the Description page of Project Settings.


#include "SpartaCharacter.h"
#include "EnhancedInputComponent.h"
#include "SpartaPlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "SpartaGameState.h"

// Sets default values
ASpartaCharacter::ASpartaCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.0f;
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(GetMesh());
	OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);

	NormalSpeed = 600.0f;
	SprintSpeedMultiplier = 1.5f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	MaxHealth = 100.0f;
	Health = MaxHealth;

	SlowMultiplier = 1.0f;
	bIsSprinting = false;
}

void ASpartaCharacter::BeginPlay()
{
	Super::BeginPlay();

	UpdateOverheadHP();
}


// Called to bind functionality to input
void ASpartaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ASpartaPlayerController* PlayerConteroller = Cast< ASpartaPlayerController>(GetController()))
		{
			if (PlayerConteroller->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerConteroller->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::Move
				);
			}
			if (PlayerConteroller->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerConteroller->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::StartJump
				);
				EnhancedInput->BindAction(
					PlayerConteroller->JumpAction,
					ETriggerEvent::Completed,
					this,
					&ASpartaCharacter::StopJump
				);
			}
			if (PlayerConteroller->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerConteroller->LookAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::Look
				);
			}
			if (PlayerConteroller->SprintAction)
			{
				EnhancedInput->BindAction(
					PlayerConteroller->SprintAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::StartSprint
				);
				EnhancedInput->BindAction(
					PlayerConteroller->SprintAction,
					ETriggerEvent::Completed,
					this,
					&ASpartaCharacter::StopSprint
				);
			}
		}
	}
}

float ASpartaCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{	
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
	UpdateOverheadHP();


	if (Health <= 0.0f)
	{
		OnDeath();
	}

	return ActualDamage;
}

void ASpartaCharacter::Move(const FInputActionValue& value)
{
	if (!Controller)
		return;

	FVector2D MoveInput = value.Get<FVector2D>();

	if (bReverseControl)
	{
		MoveInput *= -1.0f;
	}

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}
	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}

void ASpartaCharacter::StartJump(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		Jump();
	}
}

void ASpartaCharacter::StopJump(const FInputActionValue& value)
{
	if (!value.Get<bool>())
	{
		StopJumping();
	}
}

void ASpartaCharacter::Look(const FInputActionValue& value)
{

	FVector2D LookInput = value.Get<FVector2D>();

	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void ASpartaCharacter::StartSprint(const FInputActionValue& value)
{
	/*if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}*/

	bIsSprinting = true;
	UpdateMovementSpeed();
}

void ASpartaCharacter::StopSprint(const FInputActionValue& value)
{
	//if (GetCharacterMovement())
	//{
	//	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	//}

	bIsSprinting = false;
	UpdateMovementSpeed();
}

void ASpartaCharacter::OnDeath()
{
	ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState< ASpartaGameState>() : nullptr;
	if (SpartaGameState)
	{
		SpartaGameState->OnGameOver();
	}
}

void ASpartaCharacter::UpdateOverheadHP()
{
	if (!OverheadWidget)
		return;

	UUserWidget* OverheadWidgetInstance = OverheadWidget->GetUserWidgetObject();
	if (!OverheadWidgetInstance)
		return;

	if (UTextBlock* HPText = Cast<UTextBlock>(OverheadWidgetInstance->GetWidgetFromName(TEXT("OverHeadHP"))))
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth)));
	}

	if (UTextBlock* HPText = Cast<UTextBlock>(OverheadWidgetInstance->GetWidgetFromName(TEXT("OverHeadHP"))))
	{
		HPText->SetText(FText::FromString(
			FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth)
		));

		if (Health <= MaxHealth * 0.3f)
		{
			HPText->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
		}
		else if (Health <= MaxHealth * 0.7f)
		{
			HPText->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));
		}
		else
		{
			HPText->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
		}
	}
}

float ASpartaCharacter::GetHealth() const
{
	return Health;
}

void ASpartaCharacter::AddHeatlh(float Amount)
{
	Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
	UpdateOverheadHP();
}

FString ASpartaCharacter::GetDebuffStatusText() const
{
	TArray<FString> DebuffLines;

	if (bIsSlowed)
	{
		float RemainingTime = GetWorldTimerManager().GetTimerRemaining(SlowTimerHandle);
		DebuffLines.Add(FString::Printf(TEXT("Slow: %.1fs"), RemainingTime));
	}

	if (bReverseControl)
	{
		float RemainingTime = GetWorldTimerManager().GetTimerRemaining(ReverseControlTimerHandle);
		DebuffLines.Add(FString::Printf(TEXT("Reverse: %.1fs"), RemainingTime));
	}

	if (DebuffLines.Num() == 0)
	{
		return TEXT("");
	}

	return FString::Join(DebuffLines, TEXT("\n"));
}

void ASpartaCharacter::UpdateMovementSpeed()
{
	const float BaseSpeed = bIsSprinting ? SprintSpeed : NormalSpeed;
	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed * SlowMultiplier;
}

void ASpartaCharacter::ApplySlow(float SlowDuration)
{
	bIsSlowed = true;

	SlowMultiplier = 0.5f;
	UpdateMovementSpeed();

	GetWorldTimerManager().ClearTimer(SlowTimerHandle);

	GetWorldTimerManager().SetTimer(
		SlowTimerHandle,
		this,
		&ASpartaCharacter::RemoveSlow,
		SlowDuration,
		false
	);
}

void ASpartaCharacter::RemoveSlow()
{
	bIsSlowed = false;

	SlowMultiplier = 1.0f;
	UpdateMovementSpeed();
}

void ASpartaCharacter::ApplyReverseControl(float ReverseDuration)
{
	bReverseControl = true;

	GetWorldTimerManager().ClearTimer(ReverseControlTimerHandle);

	GetWorldTimerManager().SetTimer(
		ReverseControlTimerHandle,
		this,
		&ASpartaCharacter::RemoveReverseControl,
		ReverseDuration,
		false
	);
}

void ASpartaCharacter::RemoveReverseControl()
{
	bReverseControl = false;
}

