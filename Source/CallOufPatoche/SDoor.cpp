// Fill out your copyright notice in the Description page of Project Settings.


#include "SDoor.h"
#include "Components/SphereComponent.h"

// Sets default values
ASDoor::ASDoor()
{
	bActivatedDoor = false;
}

// Called when the game starts or when spawned
void ASDoor::BeginPlay()
{
	Super::BeginPlay();

	SphereCollision->SetGenerateOverlapEvents(bActivatedDoor);

}

void ASDoor::Interaction_Implementation(APlayerController* PC)
{
}

