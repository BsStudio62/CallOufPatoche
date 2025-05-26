// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameModeWave.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnActorKilled, AActor*, VictimActor, AActor*, KillerActor, AController*, KillerController, ASPlayerState*, KillerPlayerState);

class ASSpawnPoint;

UCLASS()
class CALLOUFPATOCHE_API ASGameModeWave : public AGameModeBase
{
	GENERATED_BODY()

public:

	ASGameModeWave();

protected:

	bool Init();

	void PlayersLocationSpawnPoint();

	UPROPERTY()
	TArray<ASSpawnPoint*> SpawnPoints;

	UPROPERTY()
	TArray<ASSpawnPoint*> SpawnPointsPossible;

	UPROPERTY()
	TArray<APlayerController*> PlayersControllers;

	UPROPERTY(EditDefaultsOnly)
	float DistanceSpawner;

	void CheckPlayerAlive();

	void CheckAIAlive();

	void GameOver();

	void Respawn();

	UFUNCTION(BlueprintImplementableEvent)
	void SpawnAI(FTransform Transform);

#pragma region // Wave System

	FTimerHandle TimerHandle_Launch;

	FTimerHandle TimerHandle_BotSpawn;

	FTimerHandle TimerHandle_BetweenWave;

	UPROPERTY(EditDefaultsOnly, Category = "Waves")
	float LaunchDelayStart;

	UPROPERTY(EditDefaultsOnly, Category = "Waves")
	float DelayBetweenBotSpawn;

	UPROPERTY(EditDefaultsOnly, Category = "Waves")
	float DelayBetweenWaves;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int32 WaveCount;

	UPROPERTY(VisibleAnywhere)
	int32 BotsSpawn;

	UPROPERTY(VisibleAnywhere)
	TArray<AActor*>Bots;

	void LaunchWaveSystem();

	void PrepareNextWave();

	void StartWave();

	void EndWave();

	void SpawnBot();

#pragma endregion

public:

	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	UFUNCTION(BlueprintCallable)
	void AddRemoveBots(bool Add, AActor* Actor);

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "GameMode")
	FOnActorKilled OnActorKilled;
	
};
