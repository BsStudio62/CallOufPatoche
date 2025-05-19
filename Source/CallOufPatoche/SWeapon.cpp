// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
// Input
#include "Input/SInputConfigWeapon.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "SCharacter.h"
// Access Collision & Surface
#include "CallOufPatoche.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "SPlayerController.h"
#include "Widget/SHud.h"
// Access Macro Multiplayer
#include "SAttributeComponent.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogTemplateWeapon);

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(TEXT("Weapon.DebugWeapons"),DebugWeaponDrawing,TEXT("Draw Debug Lines for Weapons"),ECVF_Cheat);

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

	bIkAds = false;

	//Setting Weapon
	BulletSpread = 2.0f;
	RateOfFire = 600;
	DistanceFire = 10000.0f;
	NbFireBurstMax = 3;
	NbFireBurst = 1;
	WeaponDamage = 10.0f;

	//Munition
	Munition = 100;
	MunitionMagazine = 100;
	MunitionMagazineMax = 100;

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
	const USAttributeComponent* AC = USAttributeComponent::GetComponentAttribute(GetOwner());
	
	if (AC && AC->GetPlayerIsAlive()) return;
	
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

	ASCharacter* Char = Cast<ASCharacter>(MyOwner);

	ASPlayerController* PC = MyOwner->GetInstigatorController<ASPlayerController>();

	if (MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;

		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();

		// Remove Bullet Spread in Aiming
		if (!bAiming)
		{
			// Bullet Spread
			float HalfRad = FMath::DegreesToRadians(BulletSpread);
			ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);
		}


		FVector TraceEnd = EyeLocation + (ShotDirection * DistanceFire);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		FHitResult Hit;

		EPhysicalSurface SurfaceType = SurfaceType_Default;
		FColor ColorHit;

		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
		{
	
			float Damage;

			//@TODO Logical system hit and damage
			AActor* HitActor = Hit.GetActor();

			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			if (SurfaceType == SURFACE_FLESHVULNERABLE)
			{
				ColorHit = FColor::Red;

				Damage = WeaponDamage * 10.0f;

				UE_LOG(LogTemp, Log, TEXT("Weapon Damage: %s"), *FString::SanitizeFloat(WeaponDamage));
			}
			else
			{
				ColorHit = FColor::Blue;

				Damage = WeaponDamage;
			}

			if (DebugWeaponDrawing > 0)
			{
				DrawDebugSphere(GetWorld(), Hit.Location, 10.0f, 10, ColorHit, false, 1.0f, 0, 1.0f);
			}
			

			UGameplayStatics::ApplyPointDamage(HitActor, Damage, ShotDirection, Hit, MyOwner->GetInstigatorController(), GetOwner(), UDamageType::StaticClass());

		}

		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
		}

		if (PC && CameraShakeFire)
		{
			PC->ClientStartCameraShake(CameraShakeFire);
		}
	
		// Aiming
		if (bAiming)
		{
			if (!bIkAds)
			{
				NetMulticast_PlayAnimation(WeaponFireAdsFPS, WeaponFireTPS, nullptr, ETypeAnimation::FIRESIGHT);
			}

		}
		else
		{
			NetMulticast_PlayAnimation(WeaponFireFPS, WeaponFireTPS, nullptr, ETypeAnimation::FIRE);
		}

		// Play sound
		if (Char)
		{
			UGameplayStatics::SpawnSoundAttached(FireSound, Char->GetMesh1P(), Socket, FVector(ForceInit), EAttachLocation::SnapToTarget);
		}

		LastFireTime = GetWorld()->TimeSeconds;

		Munition--;

		PC->Client_UpdateHudEnum(EUpdateHud::Munition, true, nullptr);

		OnFire.Broadcast();

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
	
	if (MunitionMagazine == 0 || bReloading == true || Munition == MagazineCapacity) return;

	bReloading = true;

	UE_LOG(LogTemplateWeapon, Error, TEXT("'%s' Reloading"), *GetNameSafe(this));

	if (HasAuthority())
	{
		NetMulticast_PlayAnimation(WeaponReloadFPS, WeaponReloadTPS, nullptr, ETypeAnimation::RELOAD);
	}
	else
	{
		Server_PlayAnimation(WeaponReloadFPS, WeaponReloadTPS, nullptr, ETypeAnimation::RELOAD);
	}
}

void ASWeapon::PlayEffectWeapon()
{
	UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
}

void ASWeapon::NetMulticast_PlayEffectWeapon_Implementation()
{
	PlayEffectWeapon();
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

	// Montage Weapon FPS // Replace animation montage FPS
	//PlayAnimationMontage(Weapon->GetAnimInstance(), AnimationMontageWeapon, ETypeAnimation::NONE);

	// Montage Weapon TPS //

	if (FakeWeaponTP)
	{
		PlayAnimationMontage(FakeWeaponTP->GetWeaponMesh()->GetAnimInstance(), AnimationMontageWeapon, ETypeAnimation::NONE);
	}
	

}

void ASWeapon::PlayAnimationMontage(UAnimInstance* AnimationInstance, UAnimMontage* AnimationMontage, ETypeAnimation TypeAnimation)
{
	if (AnimationInstance && AnimationMontage)
	{
		AnimationInstance->Montage_Play(AnimationMontage);

		switch (TypeAnimation)
		{
		case ETypeAnimation::RELOAD:

			if (!AnimationInstance->OnPlayMontageNotifyBegin.IsBound())
			{
				AnimationInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ASWeapon::MontageNotifyBeginReload);
			}

			if (!AnimationInstance->OnMontageEnded.IsBound())
			{
				
				AnimationInstance->OnMontageEnded.AddDynamic(this, &ASWeapon::MontageEndedReload);
			}	

			AnimationInstanceFPS = AnimationInstance;

			break;
		default:
			break;
		}

		
	}
}

bool ASWeapon::CheckMunition()
{
	return Munition == 0;
}

int32 ASWeapon::CalculateMunition()
{
	// Calcul du nombre de balles � recharger
	int32 MunitionRemain = MagazineCapacity - Munition;

	if (MunitionMagazine >= MunitionRemain)
	{
		// Assez de munitions pour un rechargement complet
		MunitionMagazine -= MunitionRemain;
		return MunitionRemain;
	}
	else
	{
		// On recharge ce qu'on peut avec les munitions restantes
		int32 MunitionUsed = MunitionMagazine;
		MunitionMagazine = 0;
		return MunitionUsed;
	}

	return 0;
}

void ASWeapon::RefillMunitionMax(AActor* OwnerChar)
{

	ASCharacter* Pawn = Cast<ASCharacter>(OwnerChar);

	if (Pawn)
	{
		ASWeapon* Weapon = Pawn->GetWeapon();
			
		if (Weapon)
		{
			Weapon->Munition = Weapon->MagazineCapacity;
			Weapon->MunitionMagazine = Weapon->MunitionMagazineMax;
			Weapon->OnFire.Broadcast();

			ASPlayerController* PC = Pawn->GetInstigatorController<ASPlayerController>();

			if (PC) PC->Client_UpdateHudEnum(EUpdateHud::Munition, true, nullptr);

		}
		
	}

	
}

void ASWeapon::SetupInputSystem()
{
	// Set up action bindings
	if (GetOwner())
	{

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
	else
	{
		UE_LOG(LogTemplateWeapon, Error, TEXT("'%s' Owner not valid"), *GetNameSafe(this));
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

		Munition += CalculateMunition();

		OnFire.Broadcast();

		ASPlayerController* PC = GetOwner()->GetInstigatorController<ASPlayerController>();

		if (PC) PC->Client_UpdateHudEnum(EUpdateHud::Munition, true, nullptr);

		bReloading = false;		

		// Unbound Dynamic
		AnimationInstanceFPS->OnMontageEnded.RemoveDynamic(this, &ASWeapon::MontageEndedReload);
	}
	
}

void ASWeapon::MontageNotifyBeginReload(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	UE_LOG(LogTemplateWeapon, Log, TEXT("MontageNotifyBeginReload triggered with NotifyName: %s"), *NotifyName.ToString());
	//AnimationInstanceFPS->OnPlayMontageNotifyBegin.RemoveDynamic(this, &ASWeapon::MontageNotifyBeginReload);
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
	const USAttributeComponent* AC = USAttributeComponent::GetComponentAttribute(GetOwner());
	
	if (AC && AC->GetPlayerIsAlive()) return;
	
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
