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

#pragma region Input

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* WeaponMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	USInputConfigWeapon* InputActions;

#pragma endregion 

	/** Weapon Mesh */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Weapon;

	ASWeapon* FakeWeaponTP;

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

	void PlayAnimation();

	// Verification Munition not egal 0 //
	bool CheckMunition();

	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#pragma region // Animation

	UPROPERTY(EditDefaultsOnly, category = "Animation")
	TSubclassOf<UAnimInstance> AnimationLayerFP;

	UPROPERTY(EditDefaultsOnly, category = "Animation")
	TSubclassOf<UAnimInstance> AnimationLayerTP;

	UPROPERTY(EditDefaultsOnly, category = "Animation")
	UAnimMontage* WeaponFire;

	UPROPERTY(EditDefaultsOnly, category = "Animation")
	UAnimMontage* WeaponFireFP;

	UPROPERTY(EditDefaultsOnly, category = "Animation")
	UAnimMontage* WeaponFireTP;

#pragma endregion 

public:	

	void HideFakeWeapon(bool HideFakeWeapon);

	void StartFire();

	void StopFire();

	void StartReload();

	void SetupInputSystem();


#pragma region // Get

	FName GetSocket() const { return Socket; }

	UFUNCTION(BlueprintPure)
	USkeletalMeshComponent* GetWeaponMesh() const	{ return Weapon; }

	TSubclassOf<UAnimInstance> GetAnimationLayerFP() const { return AnimationLayerFP; }

	TSubclassOf<UAnimInstance> GetAnimationLayerTP() const { return AnimationLayerTP; }


#pragma endregion 


	void SetFakeWeapon(ASWeapon* WeaponValue) { FakeWeaponTP = WeaponValue; }


};
