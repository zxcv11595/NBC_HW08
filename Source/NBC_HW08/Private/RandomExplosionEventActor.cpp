#include "RandomExplosionEventActor.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"

ARandomExplosionEventActor::ARandomExplosionEventActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ARandomExplosionEventActor::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoStart)
	{
		StartExplosionEvent();
	}
}

void ARandomExplosionEventActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopExplosionEvent();

	Super::EndPlay(EndPlayReason);
}

void ARandomExplosionEventActor::StartExplosionEvent()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	World->GetTimerManager().ClearTimer(ExplosionEventTimerHandle);
	World->GetTimerManager().ClearTimer(ExplosionDelayTimerHandle);

	bExplosionPending = false;

	World->GetTimerManager().SetTimer(
		ExplosionEventTimerHandle,
		this,
		&ARandomExplosionEventActor::CreateRandomExplosionPoint,
		EventInterval,
		true,
		2.0f
	);
}

void ARandomExplosionEventActor::StopExplosionEvent()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	World->GetTimerManager().ClearTimer(ExplosionEventTimerHandle);
	World->GetTimerManager().ClearTimer(ExplosionDelayTimerHandle);

	bExplosionPending = false;
}

void ARandomExplosionEventActor::CreateRandomExplosionPoint()
{
	if (bExplosionPending)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);
	if (!PlayerPawn)
	{
		return;
	}

	const FVector PlayerLocation = PlayerPawn->GetActorLocation();

	const float RandomAngle = FMath::RandRange(0.0f, 360.0f);
	const float RandomDistance = FMath::RandRange(
		MinDistanceFromPlayer,
		MaxDistanceFromPlayer
	);

	const float Radian = FMath::DegreesToRadians(RandomAngle);

	const FVector RandomDirection = FVector(
		FMath::Cos(Radian),
		FMath::Sin(Radian),
		0.0f
	);

	PendingExplosionLocation = PlayerLocation + RandomDirection * RandomDistance;
	PendingExplosionLocation.Z = PlayerLocation.Z;

	bExplosionPending = true;

	if (bDrawDebug)
	{
		DrawDebugSphere(
			World,
			PendingExplosionLocation,
			ExplosionRadius,
			32,
			FColor::Red,
			false,
			WarningDuration,
			0,
			3.0f
		);
	}

	if (WarningParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			World,
			WarningParticle,
			PendingExplosionLocation,
			FRotator::ZeroRotator,
			FVector(1.0f),
			true
		);
	}

	if (WarningSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			World,
			WarningSound,
			PendingExplosionLocation
		);
	}

	World->GetTimerManager().SetTimer(
		ExplosionDelayTimerHandle,
		this,
		&ARandomExplosionEventActor::ExecuteExplosion,
		WarningDuration,
		false
	);
}

void ARandomExplosionEventActor::ExecuteExplosion()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);
	if (!PlayerPawn)
	{
		bExplosionPending = false;
		return;
	}

	const FVector PlayerLocation = PlayerPawn->GetActorLocation();

	const float DistanceToExplosion = FVector::Dist2D(
		PlayerLocation,
		PendingExplosionLocation
	);

	if (ExplosionParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			World,
			ExplosionParticle,
			PendingExplosionLocation,
			FRotator::ZeroRotator,
			FVector(5.0f),
			true
		);
	}

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			World,
			ExplosionSound,
			PendingExplosionLocation
		);
	}

	if (DistanceToExplosion <= ExplosionRadius)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(World, 0);

		UGameplayStatics::ApplyDamage(
			PlayerPawn,
			ExplosionDamage,
			PlayerController,
			this,
			UDamageType::StaticClass()
		);
	}

	bExplosionPending = false;
}