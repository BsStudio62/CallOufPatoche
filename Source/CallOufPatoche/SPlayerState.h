// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class CALLOUFPATOCHE_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	ASPlayerState();

protected:

	int32 Kills;

	int32 Headshots;

	UPROPERTY(ReplicatedUsing = OnRep_Points, EditAnywhere, BlueprintReadWrite)
	int32 Points;

	UFUNCTION()
	void OnRep_Points();

public:

	UFUNCTION(BlueprintPure)
	int32 GetPoints() { return Points; }

#pragma region // Set

	UFUNCTION(BlueprintCallable)
	void AddScores(float ScoreDelta) {SetScore( GetScore() + ScoreDelta); }

	UFUNCTION(BlueprintCallable)
	void AddKills(int32 Kill) { Kills += Kill; }

	UFUNCTION(BlueprintCallable)
	void AddHeashots(int32 HeadShot) { Headshots += HeadShot; }

	UFUNCTION(BlueprintCallable)
	void AddPoints(int32 Point);

	UFUNCTION(BlueprintCallable)
	void RemovePoints(int32 Point) { Points -= Point; }

#pragma endregion
	
};
