// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SInteractable.h"
#include "SDoor.generated.h"

class ASSpawnPoint;

UCLASS()
class CALLOUFPATOCHE_API ASDoor : public ASInteractable
{
	GENERATED_BODY()

	
public:	
	// Sets default values for this actor's properties
	ASDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ASSpawnPoint* SpawnPoint;

	UPROPERTY(EditDefaultsOnly)
	bool bActivatedDoor;

public:

	virtual void Interaction_Implementation(APlayerController* PC) override;

};
