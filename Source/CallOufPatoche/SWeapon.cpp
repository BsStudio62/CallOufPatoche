// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
//Access Macro Multiplayer
#include "Net/UnrealNetwork.h"

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

}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	
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

void ASWeapon::HideFakeWeapon(bool HideFakeWeapon)
{
	if (HasAuthority())
	{
		Weapon->SetOnlyOwnerSee(false);
		Weapon->SetOwnerNoSee(true);
		FakeWeapon = HideFakeWeapon;
	}
	
}

void ASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASWeapon, FakeWeapon);

}
