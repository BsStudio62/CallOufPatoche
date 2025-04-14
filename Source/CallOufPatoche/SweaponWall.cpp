// Fill out your copyright notice in the Description page of Project Settings.


#include "SweaponWall.h"
#include "Components/SphereComponent.h"


// Sets default values
ASweaponWall::ASweaponWall()
{

	Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(SphereCollision);

}

// Called when the game starts or when spawned
void ASweaponWall::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASweaponWall::Interaction_Implementation(APlayerController* PC)
{
}
