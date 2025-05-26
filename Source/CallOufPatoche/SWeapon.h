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
	FIRESIGHT		UMETA(DisplayName = "Fire Sight"),
	RELOAD	UMETA(DisplayName = "Reload"),

};

UENUM(BlueprintType)
enum class EFireMode : uint8
{
	SEMI		UMETA(DisplayName = "Semi"),
	RAFALE	UMETA(DisplayName = "Rafale"),
	AUTO      UMETA(DisplayName = "Auto"),
};

USTRUCT(BlueprintType)
struct FWeapon
{
	GENERATED_BODY()

public:
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	FString NameWeapon;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	int32 MunitionSave;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	int32 MunitionMagazineSave;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	TSubclassOf<ASWeapon> WeaponClass;
		
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
	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
	USkeletalMeshComponent* Weapon;

	// Ik System
	UPROPERTY(EditDefaultsOnly,  Category = "IK | ADS")
	FTransform HandLIkAds;

	UPROPERTY(EditDefaultsOnly, Category = "IK | ADS")
	FTransform HandRIkAds;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IK | ADS")
	bool bIkAds;

	ASWeapon* FakeWeaponTP;

	// Socket Weapon Attach
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
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

	void PlayEffectWeapon();

	// SFX
	UPROPERTY(EditdefaultsOnly, Category = "SFX")
	USoundBase* FireSound;

	//CameraShake
	UPROPERTY(EditdefaultsOnly, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> CameraShakeFire;

#pragma region // Network 

	UFUNCTION(Server, Reliable)
	void Server_Fire();

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_PlayAnimation(UAnimMontage* AnimationMontageFPS, UAnimMontage* AnimationMontageTPS, UAnimMontage* AnimationMontageWeapon, ETypeAnimation TypeAnimation);

	UFUNCTION(Server, Reliable)
	void Server_PlayAnimation(UAnimMontage* AnimationMontageFPS, UAnimMontage* AnimationMontageTPS, UAnimMontage* AnimationMontageWeapon, ETypeAnimation TypeAnimation);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_PlayEffectWeapon();

#pragma endregion 

	FTimerHandle TimerHandle_TimeBetweenShots;

	int32 NbFireBurst;

	UPROPERTY(EditDefaultsOnly)
	int32 NbFireBurstMax;

	float LastFireTime;

	float TimeBetweenShots;

	UPROPERTY(EditDefaultsOnly, category = "Setting Weapon")
	float RateOfFire;

	UPROPERTY(EditDefaultsOnly, category = "Setting Weapon")
	float BulletSpread;

	UPROPERTY(EditDefaultsOnly, category = "Setting Weapon")
	float DistanceFire;

	UPROPERTY(EditDefaultsOnly, category = "Setting Weapon")
	float WeaponDamage;

	UPROPERTY(BlueprintReadOnly)
	bool bAiming;

	bool bReloading;

#pragma region // Munition

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Munition")
	int32 Munition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Munition")
	int32 MunitionMagazine;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Munition")
	int32 MunitionMagazineMax;

	UPROPERTY(EditDefaultsOnly, Category = "Munition")
	int32 MagazineCapacity;

	// Check Munition not egal 0 //
	bool CheckMunition();

	int32 CalculateMunition();

	public : 

	UFUNCTION(BlueprintCallable)
	static void RefillMunitionMax(AActor* OwnerChar);

#pragma endregion

	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#pragma region // Animation

	UPROPERTY(EditDefaultsOnly, category = "Animation | Layers")
	TSubclassOf<UAnimInstance> AnimationLayerFPS;

	UPROPERTY(EditDefaultsOnly, category = "Animation | Layers")
	TSubclassOf<UAnimInstance> AnimationLayerTPS;

	UPROPERTY(EditDefaultsOnly, category = "Animation | Fire")
	UAnimMontage* WeaponFireFPS;

	UPROPERTY(EditDefaultsOnly, category = "Animation | Fire")
	UAnimMontage* WeaponFireTPS;

	UPROPERTY(EditDefaultsOnly, category = "Animation | FireAds")
	UAnimMontage* WeaponFireAdsFPS;

	UPROPERTY(EditDefaultsOnly, category = "Animation | Reload")
	UAnimMontage* WeaponReloadFPS;

	UPROPERTY(EditDefaultsOnly, category = "Animation | Reload")
	UAnimMontage* WeaponReloadTPS;
	//----------------------------------

	UAnimInstance* AnimationInstanceFPS;

	void PlayAnimation(UAnimMontage* AnimationMontageFPS, UAnimMontage* AnimationMontageTPS, UAnimMontage* AnimationMontageWeapon, ETypeAnimation TypeAnimation);

	void PlayAnimationMontage(UAnimInstance* AnimationInstance, UAnimMontage* AnimationMontage, ETypeAnimation TypeAnimation);

	UFUNCTION()
	virtual void MontageEndedReload(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	virtual void MontageNotifyBeginReload(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);

#pragma endregion 

public:	

	void HideFakeWeapon(bool HideFakeWeapon);

	void StartFire();

	void StopFire();

	void StartReload();

	void StartAim();

	void StopAim();

	UFUNCTION(BlueprintCallable)
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

	UAnimInstance* GetAnimationInstance() const { return Weapon->GetAnimInstance(); }

#pragma endregion 

	void SetFakeWeapon(ASWeapon* WeaponValue) { FakeWeaponTP = WeaponValue; }

};
