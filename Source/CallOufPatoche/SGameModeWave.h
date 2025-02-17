// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameModeWave.generated.h"

class ASSpawnPoint;

UCLASS()
class CALLOUFPATOCHE_API ASGameModeWave : public AGameModeBase
{
	GENERATED_BODY()

	ASGameModeWave();

	bool Init();

	void PlayersLocationSpawnPoint();

	TArray<ASSpawnPoint*> SpawnPoints;

	TArray<ASSpawnPoint*> SpawnPointsPossible;

	TArray<APawn*> Players;

	UPROPERTY(EditDefaultsOnly)
	float DistanceSpawner;

public:

	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;
	
};
