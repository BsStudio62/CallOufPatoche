// Fill out your copyright notice in the Description page of Project Settings.


#include "SDoor.h"
#include "Components/SphereComponent.h"
#include "SCharacter.h"
#include "CallOufPatoche.h"
#include "Widget/SHud.h"
#include "SPlayerController.h"

// Sets default values
ASDoor::ASDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));

	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SphereCollision->SetCollisionResponseToChannel(COLLISION_PLAYER, ECR_Overlap);

	bReplicates = true;

}

// Called when the game starts or when spawned
void ASDoor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ASDoor::BeginOverlap);
		SphereCollision->OnComponentEndOverlap.AddDynamic(this, &ASDoor::EndOverlap);
	}
	
}

void ASDoor::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Error, TEXT("'%s' '%s' Begin "), *GetNameSafe(GetOwner()), *GetNameSafe(this));

	ASCharacter* Player = Cast<ASCharacter>(OtherActor);

	if (Player)
	{
		Player->SetInteraction(this);
		ASPlayerController::GetPlayerController(Player)->Client_UpdateHudEnum(EUpdateHud::Interaction, true, this);
	}
}

void ASDoor::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//UE_LOG(LogTemp, Error, TEXT("'%s' '%s' End "), *GetNameSafe(GetOwner()), *GetNameSafe(this));

	ASCharacter* Player = Cast<ASCharacter>(OtherActor);

	if (Player)
	{
		Player->SetInteraction(nullptr);
		ASPlayerController::GetPlayerController(Player)->Client_UpdateHudEnum(EUpdateHud::Interaction, false, nullptr);
	}
}

// Called every frame
void ASDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASDoor::Interaction_Implementation(APlayerController* PC)
{
	Interaction(PC);
}

