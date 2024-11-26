// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

class USkeletalMeshComponent;

UCLASS()
class CALLOUFPATOCHE_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Weapon;

	UPROPERTY(EditDefaultsOnly)
	FName Socket;

	UPROPERTY(ReplicatedUsing = OnRep_HideFakeWeapon, VisibleAnywhere)
	bool FakeWeapon;

	UFUNCTION()
	virtual void OnRep_HideFakeWeapon();

public:	

	FName GetSocket() const { return Socket; }

	UFUNCTION(BlueprintPure)
	USkeletalMeshComponent* GetWeaponMesh() { return Weapon; }

	void HideFakeWeapon(bool HideFakeWeapon);

};
