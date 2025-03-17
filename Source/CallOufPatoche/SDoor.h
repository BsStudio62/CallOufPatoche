// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/SInterface.h"
#include "SDoor.generated.h"

class USphereComponent;
class ASSpawnPoint;

UCLASS()
class CALLOUFPATOCHE_API ASDoor : public AActor, public ISInterface
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere)
	USphereComponent* SphereCollision;
	
public:	
	// Sets default values for this actor's properties
	ASDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ASSpawnPoint* SpawnPoint;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Interaction_Implementation(APlayerController* PC) override;

	UFUNCTION(BlueprintImplementableEvent)
	void Interaction(APlayerController* PC);
};
