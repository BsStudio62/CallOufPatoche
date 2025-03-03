// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"

ASPlayerState::ASPlayerState()
{
	Kills = 0;

	Headshots = 0;

	Points = 0;

}

void ASPlayerState::AddPoints(int32 Point)
{
	Points += Point;
	AddScores(Point);
}