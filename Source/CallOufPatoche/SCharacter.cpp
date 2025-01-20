// Copyright Epic Games, Inc. All Rights Reserved.

#include "SCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "SWeapon.h"
#include "Input/SInputConfigCharacter.h"
//Access Macro Multiplayer
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ATP_FirstPersonCharacter

ASCharacter::ASCharacter()
{
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	Mesh1P->SetRelativeLocation(FVector(-10.f, 0.f, -158.f));


	// Create scene component for IK system
	HandL = CreateDefaultSubobject<USceneComponent>(TEXT("HandL"));
	HandL->SetupAttachment(Mesh1P);
	HandR = CreateDefaultSubobject<USceneComponent>(TEXT("HandR"));
	HandR->SetupAttachment(Mesh1P);
	HandLAds = CreateDefaultSubobject<USceneComponent>(TEXT("HandLAds"));
	HandLAds->SetupAttachment(Mesh1P);
	HandRAds = CreateDefaultSubobject<USceneComponent>(TEXT("HandRAds"));
	HandRAds->SetupAttachment(Mesh1P);
	

}

void ASCharacter::CreateWeapon()
{

	// Create Weapon server
	if (HasAuthority())
	{
		if (!WeaponStarterClass) return;

		FActorSpawnParameters Params;
		Params.Owner = this;

		// SpawnWeapon
		CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(WeaponStarterClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);
		// Attach Weapon
		CurrentWeapon->AttachToComponent(Mesh1P, FAttachmentTransformRules::SnapToTargetNotIncludingScale, CurrentWeapon->GetSocket());

		// Spawn Fake Weapon
		FakeWeapon = GetWorld()->SpawnActor<ASWeapon>(WeaponStarterClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);
		// Attach Fake Weapon
		FakeWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FakeWeapon->GetSocket());
		FakeWeapon->HideFakeWeapon(true);

	}

	FTimerHandle TimerHandleDelay;
	GetWorldTimerManager().SetTimer(TimerHandleDelay, this, &ASCharacter::StartDelayed, 0.5f);

}

void ASCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(CharacterMappingContext, 0);
		}
	}

	CreateWeapon();
	
}

//////////////////////////////////////////////////////////////////////////// Input

void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(InputActions->JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(InputActions->JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(InputActions->MoveAction, ETriggerEvent::Triggered, this, &ASCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(InputActions->LookAction, ETriggerEvent::Triggered, this, &ASCharacter::Look);
		
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

UAnimInstance* ASCharacter::GetAnimationInstance(EAnimationInstance AnimationInstance) const
{
	UAnimInstance* Animation = nullptr;

	switch (AnimationInstance)
	{
	case EAnimationInstance::MeshFPS:
		Animation = GetMesh1P()->GetAnimInstance();
		break;
	case EAnimationInstance::MeshTPS:
		Animation = GetMesh()->GetAnimInstance();
		break;
	case EAnimationInstance::Weapon:
		Animation = CurrentWeapon->GetWeaponMesh()->GetAnimInstance();
		break;
	default:
		break;
	}

	return Animation;
}

void ASCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ASCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ASCharacter::StartDelayed()
{
	SetupAnimationLayer();

	CurrentWeapon->SetupInputSystem();

	CurrentWeapon->SetFakeWeapon(FakeWeapon);

	InitializationIkSystem();

}

void ASCharacter::SetupAnimationLayer()
{

	if (!CurrentWeapon) return;

	UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' CurrentWeapon valid"), *GetNameSafe(this));

	//Setup Layer 1P
	Mesh1P->LinkAnimClassLayers(CurrentWeapon->GetAnimationLayerFP());

	//Setup Layer TP
	GetMesh()->LinkAnimClassLayers(CurrentWeapon->GetAnimationLayerTP());

}

void ASCharacter::InitializationIkSystem()
{
	if (CurrentWeapon)
	{
		HandLAds->SetRelativeTransform(CurrentWeapon->GetHandLIkAds());

		HandRAds->SetRelativeTransform(CurrentWeapon->GetHandRIkAds());

	}

}

void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCharacter, CurrentWeapon);
	DOREPLIFETIME(ASCharacter, FakeWeapon);
}