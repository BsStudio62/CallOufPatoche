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
enum class ETypeAnimation : uint8
{
	NONE		UMETA(DisplayName = "None"),
	FIRE		UMETA(DisplayName = "Fire"),
	RELOAD	UMETA(DisplayName = "Reload"),

};

UENUM(BlueprintType)
enum class EFireMode : uint8
{
	SEMI		UMETA(DisplayName = "Semi"),
	RAFALE	UMETA(DisplayName = "Rafale"),
	AUTO      UMETA(DisplayName = "Auto"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFire);

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* WeaponMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	USInputConfigWeapon* InputActions;

#pragma endregion 

	/** Weapon Mesh */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Weapon;

	// Ik System
	UPROPERTY(EditDefaultsOnly,  Category = "IK | ADS")
	FTransform HandLIkAds;

	UPROPERTY(EditDefaultsOnly, Category = "IK | ADS")
	FTransform HandRIkAds;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IK | ADS")
	bool bIkAds;

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

	void Reload();

	//sound
	UPROPERTY(EditdefaultsOnly)
	USoundBase* FireSound;

#pragma region // Network 

	UFUNCTION(Server, Reliable)
	void Server_Fire();

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_PlayAnimation(UAnimMontage* AnimationMontageFPS, UAnimMontage* AnimationMontageTPS, UAnimMontage* AnimationMontageWeapon, ETypeAnimation TypeAnimation);

	UFUNCTION(Server, Reliable)
	void Server_PlayAnimation(UAnimMontage* AnimationMontageFPS, UAnimMontage* AnimationMontageTPS, UAnimMontage* AnimationMontageWeapon, ETypeAnimation TypeAnimation);

#pragma endregion 

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

	UPROPERTY(BlueprintReadOnly)
	bool bAiming;

	bool bReloading;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Munition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MunitionMagazine;

	UPROPERTY(EditDefaultsOnly)
	int32 MagazineCapacity;

	void PlayAnimation(UAnimMontage* AnimationMontageFPS, UAnimMontage* AnimationMontageTPS, UAnimMontage* AnimationMontageWeapon, ETypeAnimation TypeAnimation);

	void PlayAnimationMontage(UAnimInstance* AnimationInstance, UAnimMontage* AnimationMontage, ETypeAnimation TypeAnimation);

	// Check Munition not egal 0 //
	bool CheckMunition();

	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	virtual void MontageEndedReload(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	virtual void MontageNotifyBeginReload(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);

#pragma region // Animation

	UPROPERTY(EditDefaultsOnly, category = "Animation | Layers")
	TSubclassOf<UAnimInstance> AnimationLayerFPS;

	UPROPERTY(EditDefaultsOnly, category = "Animation | Layers")
	TSubclassOf<UAnimInstance> AnimationLayerTPS;

	UPROPERTY(EditDefaultsOnly, category = "Animation | Fire")
	UAnimMontage* WeaponFire;

	UPROPERTY(EditDefaultsOnly, category = "Animation | Fire")
	UAnimMontage* WeaponFireFPS;

	UPROPERTY(EditDefaultsOnly, category = "Animation | Fire")
	UAnimMontage* WeaponFireTPS;

	UPROPERTY(EditDefaultsOnly, category = "Animation | FireAds")
	UAnimMontage* WeaponFireAdsFPS;

	//Animation Reload
	UPROPERTY(EditDefaultsOnly, category = "Animation | Reload")
	UAnimMontage* WeaponReload;

	UPROPERTY(EditDefaultsOnly, category = "Animation | Reload")
	UAnimMontage* WeaponReloadFPS;

	UPROPERTY(EditDefaultsOnly, category = "Animation | Reload")
	UAnimMontage* WeaponReloadTPS;
	//----------------------------------

	UAnimInstance* AnimationInstanceFPS;


#pragma endregion 

public:	

	void HideFakeWeapon(bool HideFakeWeapon);

	void StartFire();

	void StopFire();

	void StartReload();

	void StartAim();

	void StopAim();

	void SetupInputSystem();

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Events")
	FOnFire OnFire;

#pragma region // Get

	FName GetSocket() const { return Socket; }

	UFUNCTION(BlueprintPure)
	USkeletalMeshComponent* GetWeaponMesh() const	{ return Weapon; }

	TSubclassOf<UAnimInstance> GetAnimationLayerFP() const { return AnimationLayerFPS; }

	TSubclassOf<UAnimInstance> GetAnimationLayerTP() const { return AnimationLayerTPS; }

	FTransform GetHandLIkAds() const { return HandLIkAds; }

	FTransform GetHandRIkAds() const { return HandRIkAds; }

#pragma endregion 

	void SetFakeWeapon(ASWeapon* WeaponValue) { FakeWeaponTP = WeaponValue; }

};
