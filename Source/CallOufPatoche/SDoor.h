// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/SInterface.h"
#include "SDoor.generated.h"

UCLASS()
class CALLOUFPATOCHE_API ASDoor : public AActor, public ISInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Interaction_Implementation() override;

	UFUNCTION(BlueprintImplementableEvent)
	void TesteInteraction();
};
