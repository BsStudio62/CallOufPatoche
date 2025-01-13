// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "Input/SInputConfigWeapon.h"
// Input
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "SCharacter.h"
// Access Macro Multiplayer
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogTemplateWeapon);

// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	bReplicates = true;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(RootComponent);
	Weapon->SetOnlyOwnerSee(true);
	Weapon->bCastDynamicShadow = false;
	Weapon->CastShadow = false;

	BulletSpread = 2.0f;
	RateOfFire = 600;
	DistanceFire = 10000.0f;
	NbFireBurstMax = 3;
	NbFireBurst = 1;

}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	TimeBetweenShots = 60 / RateOfFire;

}

void ASWeapon::OnRep_HideFakeWeapon()
{
	if (FakeWeapon)
	{
		Weapon->SetOnlyOwnerSee(false);
		Weapon->SetOwnerNoSee(true);
	}
	else
	{
		Weapon->SetOnlyOwnerSee(true);
		Weapon->SetOwnerNoSee(false);

	}
	
}

void ASWeapon::Fire()
{
	if (CheckMunition() || bReloading)
	{
		UE_LOG(LogTemplateWeapon, Error, TEXT("'%s' Munition 0"), *GetNameSafe(this));
		return;
	}
	else 
	{
		UE_LOG(LogTemplateWeapon, Error, TEXT("'%s' '%s' '%d'"), *GetNameSafe(GetOwner()),*GetNameSafe(this), Munition);
	}

	if (!HasAuthority())
	{
		Server_Fire();
	}

	AActor* MyOwner = GetOwner();

	if (MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;

		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();

		// Bullet Spread

		if (!bAiming)
		{
			float HalfRad = FMath::DegreesToRadians(BulletSpread);
			ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);
		}


		FVector TraceEnd = EyeLocation + (ShotDirection * DistanceFire);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);

		FHitResult Hit;

		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECC_Visibility, QueryParams))
		{
	
			//@TODO Logical system hit and damage


			

		}

	
		DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);

		NetMulticast_PlayAnimation(WeaponFireFP, WeaponFireTP, WeaponFire, ETypeAnimation::FIRE);
		
		LastFireTime = GetWorld()->TimeSeconds;

		Munition--;

		// Mode burst //

		if (FireModeSelected == EFireMode::RAFALE)
		{
			if (NbFireBurst >= NbFireBurstMax)
			{
				GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
				NbFireBurst = 1;
			}
			else
			{
				NbFireBurst++;
			}
			
		}
	}

}

void ASWeapon::Reload()
{
	
	if (Munition == MunitionMagazine || bReloading == true) return;

	bReloading = true;

	UE_LOG(LogTemplateWeapon, Error, TEXT("'%s' Reloading"), *GetNameSafe(this));

	if (HasAuthority())
	{
		NetMulticast_PlayAnimation(WeaponReloadFPS, WeaponReloadTPS, WeaponReload, ETypeAnimation::RELOAD);
	}
	else
	{
		Server_PlayAnimation(WeaponReloadFPS, WeaponReloadTPS, WeaponReload, ETypeAnimation::RELOAD);
	}
}

void ASWeapon::Server_PlayAnimation_Implementation(UAnimMontage* AnimationMontageFPS, UAnimMontage* AnimationMontageTPS, UAnimMontage* AnimationMontageWeapon, ETypeAnimation TypeAnimation)
{
	NetMulticast_PlayAnimation(AnimationMontageFPS, AnimationMontageTPS, AnimationMontageWeapon, TypeAnimation);
}

void ASWeapon::NetMulticast_PlayAnimation_Implementation(UAnimMontage* AnimationMontageFPS, UAnimMontage* AnimationMontageTPS, UAnimMontage* AnimationMontageWeapon, ETypeAnimation TypeAnimation)
{
	PlayAnimation(AnimationMontageFPS, AnimationMontageTPS, AnimationMontageWeapon, TypeAnimation);
}



void ASWeapon::Server_Fire_Implementation()
{
	Fire();
}

void ASWeapon::PlayAnimation(UAnimMontage* AnimationMontageFPS, UAnimMontage* AnimationMontageTPS, UAnimMontage* AnimationMontageWeapon, ETypeAnimation TypeAnimation)
{

	AActor* MyOwner = GetOwner();
	ASCharacter* MyChar = Cast<ASCharacter>(MyOwner);

	// Montage FPS //

	PlayAnimationMontage(MyChar->GetAnimationInstance(EAnimationInstance::MeshFPS), AnimationMontageFPS, TypeAnimation);

	// Montage TPS //

	PlayAnimationMontage(MyChar->GetAnimationInstance(EAnimationInstance::MeshTPS), AnimationMontageTPS, ETypeAnimation::NONE);

	// Montage Weapon FPS // 

	PlayAnimationMontage(Weapon->GetAnimInstance(), AnimationMontageWeapon, ETypeAnimation::NONE);

	// Montage Weapon TPS //

	PlayAnimationMontage(FakeWeaponTP->GetWeaponMesh()->GetAnimInstance(), AnimationMontageWeapon, ETypeAnimation::NONE);

}

void ASWeapon::PlayAnimationMontage(UAnimInstance* AnimationInstance, UAnimMontage* AnimationMontage, ETypeAnimation TypeAnimation)
{
	if (AnimationInstance)
	{
		AnimationInstance->Montage_Play(AnimationMontage);

		switch (TypeAnimation)
		{
		case ETypeAnimation::RELOAD:

			if (!AnimationInstance->OnMontageEnded.IsBound())
			{
				AnimationInstanceFPS = AnimationInstance;
				AnimationInstance->OnMontageEnded.AddDynamic(this, &ASWeapon::MontageEndedReload);
			}	

			break;
		default:
			break;
		}

		
	}
}

bool ASWeapon::CheckMunition()
{
	if (Munition == 0)
	{
		return true;
	}

	return false;
}

void ASWeapon::SetupInputSystem()
{
	// Set up action bindings

	if (APlayerController* PlayerController = Cast<APlayerController>(GetOwner()->GetInstigatorController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(WeaponMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(InputActions->FireAction, ETriggerEvent::Started, this, &ASWeapon::StartFire);
			EnhancedInputComponent->BindAction(InputActions->FireAction, ETriggerEvent::Completed, this, &ASWeapon::StopFire);

			// Aim
			EnhancedInputComponent->BindAction(InputActions->AimAction, ETriggerEvent::Started, this, &ASWeapon::StartAim);
			EnhancedInputComponent->BindAction(InputActions->AimAction, ETriggerEvent::Completed, this, &ASWeapon::StopAim);

			//Reload 
			EnhancedInputComponent->BindAction(InputActions->ReloadAction, ETriggerEvent::Started, this, &ASWeapon::StartReload);

		}
	}

}

void ASWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

	AActor* MyOwner = GetOwner();

	if (!MyOwner) return;

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(MyOwner->GetInstigatorController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{

			Subsystem->RemoveMappingContext(WeaponMappingContext);

		}


	}
	

}

void ASWeapon::MontageEndedReload(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		Munition = MunitionMagazine;
		bReloading = false;
		AnimationInstanceFPS->OnMontageEnded.RemoveDynamic(this, &ASWeapon::MontageEndedReload);
	}
	
}

void ASWeapon::HideFakeWeapon(bool HideFakeWeapon)
{
	if (HasAuthority())
	{
		Weapon->SetOnlyOwnerSee(false);
		Weapon->SetOwnerNoSee(true);
		FakeWeapon = HideFakeWeapon;
	}
	
}

void ASWeapon::StartFire()
{
	if (GetWorldTimerManager().IsTimerActive(TimerHandle_TimeBetweenShots))
	{
		return;
	}

	if (FireModeSelected == EFireMode::SEMI)
	{
		Fire();
	}
	else
	{
		float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

		GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASWeapon::Fire, TimeBetweenShots, true, FirstDelay);
	}


}

void ASWeapon::StopFire()
{
	if (FireModeSelected == EFireMode::AUTO)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
	}

}

void ASWeapon::StartReload()
{
	Reload();
}

void ASWeapon::StartAim()
{
	bAiming = true;
}

void ASWeapon::StopAim()
{

	bAiming = false;
}

void ASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASWeapon, FakeWeapon);

}
