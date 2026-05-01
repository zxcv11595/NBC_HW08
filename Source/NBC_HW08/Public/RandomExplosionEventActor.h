// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RandomExplosionEventActor.generated.h"

//class UParticleSystem;
//class USoundBase;

UCLASS()
class NBC_HW08_API ARandomExplosionEventActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARandomExplosionEventActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	// Level3 시작 시 자동으로 이벤트를 시작할지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion Event")
	bool bAutoStart = true;
	// 폭발 이벤트 발생 간격
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion Event")
	float EventInterval = 4.0f;
	// 경고 후 실제 폭발까지 걸리는 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion Event")
	float WarningDuration = 1.0f;
	// 플레이어로부터 최소 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion Event")
	float MinDistanceFromPlayer = 250.0f;
	// 플레이어로부터 최대 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion Event")
	float MaxDistanceFromPlayer = 700.0f;
	// 폭발 반경
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion Event")
	float ExplosionRadius = 250.0f;
	// 폭발 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion Event")
	float ExplosionDamage = 20.0f;
	// 디버그 표시 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion Event")
	bool bDrawDebug = true;
	// 화면 메시지 표시 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion Event")
	bool bShowScreenMessage = true;

	// 경고용 일반 파티클
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion Effect")
	UParticleSystem* WarningParticle;

	// 폭발용 일반 파티클
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion Effect")
	UParticleSystem* ExplosionParticle;

	// 경고음
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion Sound")
	USoundBase* WarningSound;

	// 폭발음
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion Sound")
	USoundBase* ExplosionSound;

private:
	FTimerHandle ExplosionEventTimerHandle;
	FTimerHandle ExplosionDelayTimerHandle;

	FVector PendingExplosionLocation;

	bool bExplosionPending = false;

public:
	UFUNCTION(BlueprintCallable, Category = "Explosion Event")
	void StartExplosionEvent();

	UFUNCTION(BlueprintCallable, Category = "Explosion Event")
	void StopExplosionEvent();

private:
	void CreateRandomExplosionPoint();
	void ExecuteExplosion();
	void ShowScreenMessage(const FString& Message, const FColor& Color, float Duration) const;


};
