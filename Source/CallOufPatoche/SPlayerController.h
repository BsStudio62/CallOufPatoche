// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdateHud);

UCLASS()
class CALLOUFPATOCHE_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	ASPlayerController();
		
protected:

	UPROPERTY(EditDefaultsOnly)
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> HudClass;

	UUserWidget* Hud;

	virtual void BeginPlay() override;

	void CreateWidgetHud();

	UFUNCTION(Client, reliable, BlueprintCallable)
	void Client_UpdateHud();

public:

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Events")
	FOnUpdateHud OnUpdateHud;
	
};
