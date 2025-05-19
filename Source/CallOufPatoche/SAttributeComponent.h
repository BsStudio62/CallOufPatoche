// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttributeComponent.generated.h"

UENUM(BlueprintType)
enum class EControllerPossType : uint8 {
	Player = 0 UMETA(DisplayName = "Player"),
	AI = 1  UMETA(DisplayName = "AI"),
};

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

	UPROPERTY(EditDefaultsOnly, Category = Attribute)
	EControllerPossType ControllerType;

	bool bIsDead;

	UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadOnly, Category = "HealthComponent")
	float Health;

	UFUNCTION()
	void OnRep_Health(const float OldHealth);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent")
	float DefaultHealth;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor,const float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

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
	void Dead(EControllerPossType ControllerPossType);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Revive();

	UFUNCTION(BlueprintCallable)
	void ResetHealth();
	
	UFUNCTION(BlueprintCallable)
	void Heal(float HealAmount);

	// Return player dead or not
	bool GetPlayerIsAlive() const { return bIsDead; }

};
