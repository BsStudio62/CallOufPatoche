// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeWave.h"
#include "kismet/GameplayStatics.h"
#include "SSpawnPoint.h"
#include "SCharacter.h"
#include "SAttributeComponent.h"

ASGameModeWave::ASGameModeWave()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;

	DistanceSpawner = 2500.0f;

	LaunchDelayStart = 2.0f;
	DelayBetweenBotSpawn = 1.0f;
	DelayBetweenWaves = 2.0f;
}

void ASGameModeWave::StartPlay()
{
	Super::StartPlay();

	UE_LOG(LogTemp, Error, TEXT("'%s' Start Game Mode"), *GetNameSafe(this));

	if (Init())
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' Points de Spawn valid '%d'"), *GetNameSafe(this), SpawnPoints.Num());

		SpawnPointsPossible = SpawnPoints;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' Aucun Points de Spawn Valid"), *GetNameSafe(this));
	}

	GetWorldTimerManager().SetTimer(TimerHandle_Launch, this, &ASGameModeWave::StartWave, LaunchDelayStart, false);
		
}

void ASGameModeWave::SpawnBot()
{
	int32 Index = 0;

	Index = FMath::RandRange(0, SpawnPointsPossible.Num() - 1);

	FTransform SpawnLocation = SpawnPointsPossible[Index]->GetTransform();

	SpawnAI(SpawnLocation);

	BotsSpawn--;

	if (BotsSpawn <= 0)
	{
		EndWave();
	}
}

void ASGameModeWave::PrepareNextWave()
{
	UE_LOG(LogTemp, Error, TEXT("'%s' Prepare Next Wave"), *GetNameSafe(this));

	GetWorldTimerManager().SetTimer(TimerHandle_BetweenWave, this, &ASGameModeWave::StartWave, DelayBetweenWaves, false);

	Respawn();
}

void ASGameModeWave::AddRemoveBots(bool Add, AActor* Actor)
{
	if (Add)
	{
		Bots.Add(Actor);
	}
	else
	{
		Bots.Remove(Actor);
	}

	if (Bots.IsEmpty())
	{
		PrepareNextWave();
	}
}

bool ASGameModeWave::Init()
{
	TArray<AActor*> ActorSpawnPoints;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASSpawnPoint::StaticClass(), ActorSpawnPoints);

	for (AActor* ActorSpawnPoint : ActorSpawnPoints)
	{
		ASSpawnPoint* SpawnPoint = Cast<ASSpawnPoint>(ActorSpawnPoint);

		if (SpawnPoint)
		{
			SpawnPoints.Add(SpawnPoint);
		}
	}

	return !SpawnPoints.IsEmpty();
}

void ASGameModeWave::StartWave()
{
	WaveCount++;

	BotsSpawn = WaveCount * 2;

	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawn, this, &ASGameModeWave::SpawnBot, DelayBetweenBotSpawn, true);
}

void ASGameModeWave::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawn);
}

void ASGameModeWave::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UE_LOG(LogTemp, Error, TEXT("'%s' Tick"), *GetNameSafe(this));

	PlayersLocationSpawnPoint();
	CheckPlayerAlive();
	CheckAIAlive();
}

void ASGameModeWave::PlayersLocationSpawnPoint()
{
	for ( ASSpawnPoint* SpawnPoint : SpawnPoints)
	{

		for (APlayerController* PC : PlayersControllers)
		{
			APawn* Pawn = PC->GetPawn();

			float DistanceTo = Pawn->GetDistanceTo(SpawnPoint);

			if (DistanceTo < DistanceSpawner)
			{

				if (!SpawnPointsPossible.Contains(SpawnPoint))
				{
					SpawnPointsPossible.Add(SpawnPoint);

					DrawDebugSphere(GetWorld(), SpawnPoint->GetActorLocation(), 100.0f, 12, FColor::Green, false, 1.0f, 0, 0);
				}
				
			}
			else
			{
	
				if (SpawnPointsPossible.Contains(SpawnPoint) && SpawnPointsPossible.Num() > 1)
				{
					SpawnPointsPossible.Remove(SpawnPoint);

					DrawDebugSphere(GetWorld(), SpawnPoint->GetActorLocation(), 100.0f, 12, FColor::Red, false, 1.0f, 0, 0);
				}
				
			}

		}

		

	}

	UE_LOG(LogTemp, Error, TEXT("'%s' SpawnPoints : '%d'"), *GetNameSafe(this), SpawnPointsPossible.Num());
}

void ASGameModeWave::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	PlayersControllers.Add(NewPlayer);
}

void ASGameModeWave::CheckPlayerAlive()
{

	for (APlayerController* PC : PlayersControllers)
	{
		APawn* Pawn = PC->GetPawn();

		if (Pawn)
		{
			USAttributeComponent* AttributeComponent = USAttributeComponent::GetComponentAttribute(Pawn);

			if (AttributeComponent)
			{

				if (!AttributeComponent->GetPlayerIsAlive())
				{
					return;
				}
			}
		}

	}

	GameOver();

}

void ASGameModeWave::GameOver()
{
	UE_LOG(LogTemp, Error, TEXT("'%s' GameOver"), *GetNameSafe(this));
}

void ASGameModeWave::CheckAIAlive()
{
	if (GetWorldTimerManager().IsTimerActive(TimerHandle_BetweenWave) || GetWorldTimerManager().IsTimerActive(TimerHandle_Launch)) return;

	for (AActor* Actor : Bots)
	{
		if (!Actor)
		{
			Bots.Remove(Actor);
			BotsSpawn++;
			SpawnBot();
		}
	}

}

void ASGameModeWave::Respawn()
{
	for (APlayerController* PC : PlayersControllers)
	{
		if (PC && PC->GetPawn() == nullptr) 
		{
			RestartPlayer(PC);
		}
	}
}