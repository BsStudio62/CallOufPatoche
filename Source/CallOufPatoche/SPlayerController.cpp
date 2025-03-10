// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"
#include "Blueprint/UserWidget.h"
// Input
#include "EnhancedInputSubsystems.h"

ASPlayerController::ASPlayerController()
{

	bReplicates = true;
}

void ASPlayerController::BeginPlay()
{
	Super::BeginPlay();


	if (UEnhancedInputLocalPlayerSubsystem * Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(this->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	CreateWidgetHud();
}

void ASPlayerController::CreateWidgetHud()
{
	if (IsLocalPlayerController())
	{
		Hud = CreateWidget<UUserWidget>(this, HudClass);

		if (Hud)
		{
			Hud->AddToViewport();
		}
	}
	
	
}

void ASPlayerController::Client_UpdateHud_Implementation()
{
	this->OnUpdateHud.Broadcast();
}
