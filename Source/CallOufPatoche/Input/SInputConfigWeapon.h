// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputAction.h"
#include "SInputConfigWeapon.generated.h"

/**
 * 
 */
UCLASS()
class CALLOUFPATOCHE_API USInputConfigWeapon : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* FireAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* AimAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* ReloadAction;

};
