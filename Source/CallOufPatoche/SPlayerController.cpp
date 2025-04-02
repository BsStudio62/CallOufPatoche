// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"
#include "Blueprint/UserWidget.h"
// Input
#include "EnhancedInputSubsystems.h"
#include "Widget/SHud.h"

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

ASPlayerController* ASPlayerController::GetPlayerController(AActor* Owner)
{
	return Owner->GetInstigatorController<ASPlayerController>();
}

void ASPlayerController::Client_UpdateHudEnum_Implementation(EUpdateHud UpdateHud, bool bActive, AActor* Interactable)
{
	USHud* HudLocal = Cast<USHud>(Hud);

	if (HudLocal)
	{
		HudLocal->UpdateHud(UpdateHud, bActive, Interactable);
	}
}

void ASPlayerController::Client_UpdateHud_Implementation()
{
	this->OnUpdateHud.Broadcast();
}
