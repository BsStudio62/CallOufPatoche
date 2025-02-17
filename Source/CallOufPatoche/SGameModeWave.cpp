// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeWave.h"
#include "kismet/GameplayStatics.h"
#include "SSpawnPoint.h"

ASGameModeWave::ASGameModeWave()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;

	DistanceSpawner = 2500.0f;
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

void ASGameModeWave::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UE_LOG(LogTemp, Error, TEXT("'%s' Tick"), *GetNameSafe(this));

	PlayersLocationSpawnPoint();
}

void ASGameModeWave::PlayersLocationSpawnPoint()
{
	for ( ASSpawnPoint* SpawnPoint : SpawnPoints)
	{

		for (APawn* Pawn : Players)
		{

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

	Players.Add(NewPlayer->GetPawn());
}
