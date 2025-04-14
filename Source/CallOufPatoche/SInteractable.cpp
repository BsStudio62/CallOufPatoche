// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractable.h"

// Sets default values
ASInteractable::ASInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

// Called when the game starts or when spawned
void ASInteractable::BeginPlay()
{
	Super::BeginPlay();
}

