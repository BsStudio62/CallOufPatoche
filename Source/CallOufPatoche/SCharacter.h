// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "SCharacter.generated.h"

UENUM(BlueprintType)
enum class EAnimationInstance : uint8 {
	MeshFPS = 0 UMETA(DisplayName = "MeshFPS"),
	MeshTPS = 1  UMETA(DisplayName = "MeshTPS"),
	Weapon = 2	UMETA(DisplayName = "Weapon"),
	FakeWeapon = 3 UMETA(DisplayName = "FakeWeapon")

};

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputMappingContext;
class USInputConfigCharacter;
struct FInputActionValue;
class ASWeapon;

// Add log custom
DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class CALLOUFPATOCHE_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* CharacterMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK", meta = (AllowPrivateAccess = "true"))
	USceneComponent* HandL;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK", meta = (AllowPrivateAccess = "true"))
	USceneComponent* HandR;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK", meta = (AllowPrivateAccess = "true"))
	USceneComponent* HandLAds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK", meta = (AllowPrivateAccess = "true"))
	USceneComponent* HandRAds;
	
public:

	ASCharacter();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	USInputConfigCharacter* InputActions;

	UPROPERTY()
	AActor* Interaction;

	void InteractionSystem();

#pragma region Weapon 

	// Weapon Starter //
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ASWeapon> WeaponStarterClass;

	// Weapon //
	UPROPERTY(Replicated, BlueprintReadWrite)
	ASWeapon* CurrentWeapon;

	// Fake Weapon //
	UPROPERTY(Replicated, BlueprintReadWrite)
	ASWeapon* FakeWeapon;

	void CreateWeapon();

#pragma endregion

	virtual void BeginPlay();

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void StartDelayed();

	UFUNCTION(BlueprintCallable)
	void SetupAnimationLayer();

	void InitializationIkSystem();

protected:

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:

#pragma region // Get

	/** Returns Mesh1P subobject **/
	UFUNCTION(BlueprintPure)
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UAnimInstance* GetAnimationInstance(EAnimationInstance AnimationInstance) const;

	ASWeapon* GetWeapon() const { return CurrentWeapon; }

#pragma endregion 


	void SetInteraction(AActor* ActorValue) { Interaction = ActorValue; }
	

};

