// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SSpawnPoint.generated.h"

UCLASS()
class CALLOUFPATOCHE_API ASSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASSpawnPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	bool bActivated;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool GetActivated() const { return bActivated; }

	UFUNCTION(BlueprintCallable)
	void SetActivated(bool bActivatedValue ) { bActivated = bActivatedValue; }

};
