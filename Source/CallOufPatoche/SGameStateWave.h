// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SGameStateWave.generated.h"

UCLASS()
class CALLOUFPATOCHE_API ASGameStateWave : public AGameStateBase
{
	GENERATED_BODY()



protected:

	UPROPERTY(ReplicatedUsing = OnRep_Wave)
	int32 Wave;

	UFUNCTION()
	void OnRep_Wave();
	
public:

	UFUNCTION(BlueprintPure)
	int32 GetWave() const { return Wave; }

	void IncrementeWave();
};
