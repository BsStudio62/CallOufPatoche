// Copyright Epic Games, Inc. All Rights Reserved.

#include "SGameMode.h"
#include "SCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASGameMode::ASGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
