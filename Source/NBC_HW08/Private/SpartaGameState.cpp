#include "SpartaGameState.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "SpartaCharacter.h"

ASpartaGameState::ASpartaGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	LevelDuration = 40.0f;
	MaxLevels = 3;
	CurrentLevelIndex = 0;

	CurrentWaveIndex = 0;

	Waves.Add({ 60.0f, 3, 40 });
	Waves.Add({ 45.0f, 2, 40 });
	Waves.Add({ 30.0f, 1, 40 });
}

void ASpartaGameState::BeginPlay()
{
	Super::BeginPlay();

	StartLevel();

	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle,
		this,
		&ASpartaGameState::UpdateHUD,
		0.1f,
		true
	);
}

int32 ASpartaGameState::GetScore() const
{
	return Score;
}

void ASpartaGameState::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			SpartaGameInstance->AddToScore(Amount);
		}
	}
}

void ASpartaGameState::OnGameOver()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->SetPause(true);
			SpartaPlayerController->ShowMainMenu(true);
		}
	}
}

void ASpartaGameState::StartLevel()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->ShowGameHUD();
		}
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
		}
	}

	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	CurrentWaveIndex = 0;

	StartWave();
	UpdateHUD();

	/*SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	const int32 ItemToSpawn = 40;

	for (int32 i = 0; i < ItemToSpawn; ++i)
	{
		if (FoundVolumes.Num() > 0)
		{
			ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
			if (SpawnVolume)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
				if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
				{
					++SpawnedCoinCount;
				}
			}
		}
	}

	UpdateHUD();

	GetWorldTimerManager().SetTimer(
		LevelTimerHandle,
		this,
		&ASpartaGameState::OnLevelTimeUp,
		LevelDuration,
		false
	);*/

}

void ASpartaGameState::OnLevelTimeUp()
{
	EndLevel();
}

void ASpartaGameState::EndLevel()
{
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);
	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			++CurrentLevelIndex;
			SpartaGameInstance->CurrentLevelIndex = CurrentLevelIndex;
		}
	}

	if (CurrentLevelIndex > MaxLevels)
	{
		OnGameOver();
		return;
	}
	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else
	{
		OnGameOver();
	}
}

void ASpartaGameState::OnCoinCollected()
{
	++CollectedCoinCount;
	UE_LOG(LogTemp, Warning, TEXT("GCoin Collected: %d / %d"),
		CollectedCoinCount,
		SpawnedCoinCount);

	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
	{
		//EndLevel();
		EndWave();
	}
}

void ASpartaGameState::UpdateHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
			{
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
				}

				if (UTextBlock* WaveText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Wave"))))
				{
					WaveText->SetText(FText::FromString(
						FString::Printf(TEXT("Wave: %d / %d"), CurrentWaveIndex + 1, Waves.Num())
					));
				}

				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
						if (SpartaGameInstance)
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), SpartaGameInstance->TotalScore)));
						}
					}
				}

				if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
				{
					LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), CurrentLevelIndex + 1)));
				}
			}

			if (UUserWidget* DebuffWidget = SpartaPlayerController->GetDebuffWidget())
			{
				if (UTextBlock* DebuffText = Cast<UTextBlock>(DebuffWidget->GetWidgetFromName(TEXT("DebuffText"))))
				{
					if (APawn* Pawn = PlayerController->GetPawn())
					{
						if (ASpartaCharacter* Character = Cast<ASpartaCharacter>(Pawn))
						{
							const FString DebuffStatus = Character->GetDebuffStatusText();

							DebuffText->SetText(FText::FromString(DebuffStatus));

							if (DebuffStatus.IsEmpty())
							{
								DebuffText->SetVisibility(ESlateVisibility::Collapsed);
							}
							else
							{
								DebuffText->SetVisibility(ESlateVisibility::Visible);
							}
						}
					}
				}
			}
		}
	}
}

void ASpartaGameState::StartWave()
{
	if (!Waves.IsValidIndex(CurrentWaveIndex))
	{
		EndLevel();
		return;
	}

	const FWaveData& Wave = Waves[CurrentWaveIndex];

	CurrentSpawnExecuted = 0;

	if (Wave.ItemSpawnCount <= 0)
	{
		EndWave();
		return;
	}

	float Interval = Wave.WaveDuration / Wave.ItemSpawnCount;

	UE_LOG(LogTemp, Warning, TEXT("Wave %d Start"), CurrentWaveIndex + 1);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Green,
			FString::Printf(TEXT("Wave %d Start!"), CurrentWaveIndex + 1)
		);
	}

	HandleSpawnEvent();

	GetWorldTimerManager().SetTimer(
		SpawnTimerHandle,
		this,
		&ASpartaGameState::HandleSpawnEvent,
		Interval,
		true
	);

	GetWorldTimerManager().SetTimer(
		WaveTimerHandle,
		this,
		&ASpartaGameState::EndWave,
		Wave.WaveDuration,
		false
	);

	UpdateHUD();
}

void ASpartaGameState::EndWave()
{
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);
	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);

	TArray<AActor*> FoundItems;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseItem::StaticClass(), FoundItems);

	for (AActor* Item : FoundItems)
	{
		if (IsValid(Item))
		{
			Item->Destroy();
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Wave %d End"), CurrentWaveIndex + 1);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Green,
			FString::Printf(TEXT("Wave %d End!"), CurrentWaveIndex + 1)
		);
	}

	CurrentWaveIndex++;

	if (CurrentWaveIndex < Waves.Num())
	{
		StartWave();
	}
	else
	{
		EndLevel();
	}
}

void ASpartaGameState::HandleSpawnEvent()
{
	if (!Waves.IsValidIndex(CurrentWaveIndex))
	{
		return;
	}

	const FWaveData& Wave = Waves[CurrentWaveIndex];

	SpawnItemsForWave(Wave.ItemsPerSpawn);

	CurrentSpawnExecuted++;

	if (CurrentSpawnExecuted >= Wave.ItemSpawnCount)
	{
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
	}
}

void ASpartaGameState::SpawnItemsForWave(int32 ItemSpawnCount)
{
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	if (FoundVolumes.Num() <= 0)
	{
		return;
	}

	ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
	if (!SpawnVolume)
	{
		return;
	}

	for (int32 i = 0; i < ItemSpawnCount; ++i)
	{
		AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();

		if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
		{
			++SpawnedCoinCount;
		}
	}
}
