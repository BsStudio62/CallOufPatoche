// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_PlayMontage.h"
#include "../SCharacter.h"
#include "../SWeapon.h"

void UAnimNotify_PlayMontage::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	AActor* MyOwner = MeshComp->GetOwner();

	if (!MyOwner) return;

	ASCharacter* Char = Cast<ASCharacter>(MyOwner);

	if (!Char) return;

	ASWeapon* Weapon = Char->GetWeapon();

	if (!Weapon) return;
	
	UAnimInstance* Anim = Weapon->GetAnimationInstance();

	if (!Anim) return;

	if (!AnimationMontage) return;

	Anim->Montage_Play(AnimationMontage);
	
}
