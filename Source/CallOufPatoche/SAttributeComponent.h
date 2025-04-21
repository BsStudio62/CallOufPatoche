// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttributeComponent.generated.h"

// OnHealthChanged event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, USAttributeComponent*, OwningHealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CALLOUFPATOCHE_API USAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USAttributeComponent();

	static USAttributeComponent* GetComponentAttribute(AActor* Actor);

protected:

	// Called when the game starts
	virtual void BeginPlay() override;

	bool bIsDead;

	UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadOnly, Category = "HealthComponent")
	float Health;

	UFUNCTION()
	void OnRep_Health(float OldHealth);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent")
	float DefaultHealth;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

#pragma region // Regen

	UPROPERTY(EditDefaultsOnly)
	bool bActivateRegenHealth;

	void LaunchTimerRegen();

	void RegenerationHealth();

	FTimerHandle RegenTimerHandle;

	float DelayRegen;

	float RegenHealth;

#pragma endregion

public:	

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Events")
	FOnHealthChangedSignature OnHealthChanged;

	UFUNCTION(BlueprintImplementableEvent)
	void Dead();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Revive();

	UFUNCTION(BlueprintCallable)
	void ResetHealth();
	
	UFUNCTION(BlueprintCallable)
	void Heal(float HealAmount);

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Return player dead or not
	bool GetPlayerIsAlive() { return bIsDead; }

};
