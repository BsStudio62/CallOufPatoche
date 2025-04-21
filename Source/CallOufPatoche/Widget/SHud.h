// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SHud.generated.h"

UENUM(BlueprintType)
enum class EUpdateHud : uint8
{
	None		UMETA(DisplayName = "None"),
	Wave	UMETA(DisplayName = "Wave"),
	Score	    UMETA(DisplayName = "Score"),
	Munition	    UMETA(DisplayName = "Munition"),
	Interaction		UMETA(DisplayName = "Interaction"),
	Weapon		UMETA(DisplayName = "Weapon"),
};

UCLASS()
class CALLOUFPATOCHE_API USHud : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateHud(EUpdateHud Update, bool bActive, AActor* Interactable);
	
};
