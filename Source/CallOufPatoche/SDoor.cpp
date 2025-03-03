// Fill out your copyright notice in the Description page of Project Settings.


#include "SDoor.h"

// Sets default values
ASDoor::ASDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

}

// Called when the game starts or when spawned
void ASDoor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASDoor::Interaction_Implementation()
{
	TesteInteraction();
}

