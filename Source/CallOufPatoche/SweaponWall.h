// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SInteractable.h"
#include "SweaponWall.generated.h"

UCLASS()
class CALLOUFPATOCHE_API ASweaponWall : public ASInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASweaponWall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Weapon;

public:

	virtual void Interaction_Implementation(APlayerController* PC) override;

};
