// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_PlayMontage.generated.h"

/**
 * 
 */
UCLASS(const, hidecategories = Object, collapsecategories, Config = Game, meta = (DisplayName = "Play Montage"))
class CALLOUFPATOCHE_API UAnimNotify_PlayMontage : public UAnimNotify
{
	GENERATED_BODY()
	
public:

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	// Animation montage to Play
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	TObjectPtr<UAnimMontage> AnimationMontage;

};
