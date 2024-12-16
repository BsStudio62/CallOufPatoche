// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

class USkeletalMeshComponent;
class UInputMappingContext;
class USInputConfigWeapon;

// Add log custom
DECLARE_LOG_CATEGORY_EXTERN(LogTemplateWeapon, Log, All);

UENUM(BlueprintType)
enum class EFireMode : uint8
{
	SEMI		UMETA(DisplayName = "Semi"),
	RAFALE	UMETA(DisplayName = "Rafale"),
	AUTO      UMETA(DisplayName = "Auto"),
};

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

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* WeaponMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	USInputConfigWeapon* InputActions;

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Weapon;

	UPROPERTY(EditDefaultsOnly)
	FName Socket;

	UPROPERTY(EditDefaultsOnly)
	TArray<EFireMode> FireMode;

	UPROPERTY(EditDefaultsOnly)
	EFireMode FireModeSelected;

	UPROPERTY(ReplicatedUsing = OnRep_HideFakeWeapon, VisibleAnywhere)
	bool FakeWeapon;

	UFUNCTION()
	virtual void OnRep_HideFakeWeapon();

	void Fire();

	UFUNCTION(Server, Reliable)
	void Server_Fire();

	FTimerHandle TimerHandle_TimeBetweenShots;

	int32 NbFireBurst;

	UPROPERTY(EditDefaultsOnly)
	int32 NbFireBurstMax;

	float LastFireTime;

	float TimeBetweenShots;

	UPROPERTY(EditDefaultsOnly)
	float RateOfFire;

	UPROPERTY(EditDefaultsOnly)
	float BulletSpread;

	UPROPERTY(EditDefaultsOnly)
	float DistanceFire;

	bool bAiming;

	UPROPERTY(EditDefaultsOnly)
	int32 Munition;

	UPROPERTY(EditDefaultsOnly)
	int32 MunitionMagazine;

	UPROPERTY(EditDefaultsOnly)
	int32 MagazineCapacity;

	// Verification Munition not 0
	bool CheckMunition();

	void SetupInputSystem();

	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	

public:	

	FName GetSocket() const { return Socket; }

	UFUNCTION(BlueprintPure)
	USkeletalMeshComponent* GetWeaponMesh() { return Weapon; }

	void HideFakeWeapon(bool HideFakeWeapon);

	void StartFire();

	void StopFire();

	void StartReload();

	void StartPlay();

};
