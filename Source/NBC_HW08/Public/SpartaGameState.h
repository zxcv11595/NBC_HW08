// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SpartaGameState.generated.h"

USTRUCT(BlueprintType)
struct FWaveData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WaveDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemSpawnCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemsPerSpawn;
};

UCLASS()
class NBC_HW08_API ASpartaGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	ASpartaGameState();

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 Score;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 SpawnedCoinCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 CollectedCoinCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Level")
	float LevelDuration;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Level")
	int32 CurrentLevelIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Level")
	int32 MaxLevels;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<FName> LevelMapNames;

	FTimerHandle LevelTimerHandle;
	FTimerHandle HUDUpdateTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	TArray<FWaveData> Waves;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 CurrentWaveIndex;

	FTimerHandle WaveTimerHandle;
	FTimerHandle SpawnTimerHandle;

	int32 CurrentSpawnExecuted = 0;

	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore() const;
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Level")
	void OnGameOver();

	void StartLevel();
	void OnLevelTimeUp();
	void EndLevel();
	void OnCoinCollected();

	void UpdateHUD();

	void StartWave();
	void EndWave();
	void SpawnItemsForWave(int32 ItemSpawnCount);
	void HandleSpawnEvent();
};
