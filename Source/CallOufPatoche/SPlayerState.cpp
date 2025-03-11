// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"
#include "SPlayerController.h"
// Access Macro Multiplayer
#include "Net/UnrealNetwork.h"

ASPlayerState::ASPlayerState()
{
	Kills = 0;

	Headshots = 0;

	Points = 0;

	NetUpdateFrequency = 5.0f;

}

void ASPlayerState::OnRep_Points()
{
	ASPlayerController* PC = Cast<ASPlayerController>(GetPlayerController());

	PC->OnUpdateHud.Broadcast();
}

void ASPlayerState::AddPoints(int32 Point)
{
	Points += Point;
	AddScores(Point);
}

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerState, Points);

}
