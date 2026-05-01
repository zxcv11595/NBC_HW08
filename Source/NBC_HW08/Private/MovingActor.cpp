// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingActor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AMovingActor::AMovingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(SceneComponent);

	MovingSpeed = 100.0f;
	MaxMoveDistance = 500.0f;
	MovingDirection = FVector(0.0f, 0.0f, 0.0f);
}

// Called when the game starts or when spawned
void AMovingActor::BeginPlay()
{
	Super::BeginPlay();
	
	StartLocation = GetActorLocation();
}

// Called every frame
void AMovingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Move(DeltaTime);
}

void AMovingActor::Move(float DeltaTime)
{
	FVector CurrentLocation = GetActorLocation();

	CurrentLocation += MovingDirection * MovingSpeed * DeltaTime;

	SetActorLocation(CurrentLocation);

	const float MovedDistance = FVector::Dist(StartLocation, CurrentLocation);

	if (MovedDistance >= MaxMoveDistance)
	{
		FVector TargetLocation = StartLocation + MovingDirection * MaxMoveDistance;
		SetActorLocation(TargetLocation);

		StartLocation = TargetLocation;
		MovingDirection *= -1.0f;
	}
}