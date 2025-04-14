// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractable.h"
#include "Components/SphereComponent.h"
#include "SCharacter.h"
#include "CallOufPatoche.h"
#include "Widget/SHud.h"
#include "SPlayerController.h"

// Sets default values
ASInteractable::ASInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));

	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SphereCollision->SetCollisionResponseToChannel(COLLISION_PLAYER, ECR_Overlap);

	SphereCollision->SetupAttachment(RootComponent);

	bReplicates = true;
}

// Called when the game starts or when spawned
void ASInteractable::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ASInteractable::BeginOverlap);
		SphereCollision->OnComponentEndOverlap.AddDynamic(this, &ASInteractable::EndOverlap);
	}
}

void ASInteractable::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{


	ASCharacter* Player = Cast<ASCharacter>(OtherActor);

	if (Player)
	{
		Player->SetInteraction(this);
		ASPlayerController::GetPlayerController(Player)->Client_UpdateHudEnum(EUpdateHud::Interaction, true, this);
	}
}

void ASInteractable::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	ASCharacter* Player = Cast<ASCharacter>(OtherActor);

	if (Player)
	{
		Player->SetInteraction(nullptr);
		ASPlayerController::GetPlayerController(Player)->Client_UpdateHudEnum(EUpdateHud::Interaction, false, nullptr);
	}
}

void ASInteractable::Interaction_Implementation(APlayerController* PC)
{

}

