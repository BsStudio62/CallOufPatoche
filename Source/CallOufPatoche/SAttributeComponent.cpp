// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"
#include "SGameModeWave.h"
#include "SPlayerState.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	DefaultHealth = 100;
	bIsDead = false;

	bActivateRegenHealth = false;

	RegenHealth = 1.0f;
	DelayRegen = 1.0f;

	SetIsReplicatedByDefault(true);
}

USAttributeComponent* USAttributeComponent::GetComponentAttribute(AActor* Actor)
{
	USAttributeComponent* AttributeComponent = Actor->GetComponentByClass<USAttributeComponent>();

	return AttributeComponent ? AttributeComponent : nullptr;
}

// Called when the game starts
void USAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority)
	{
		
		if (AActor* MyOwner = GetOwner())
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &USAttributeComponent::HandleTakeAnyDamage);
		}
	}

	Health = DefaultHealth;
}

void USAttributeComponent::OnRep_Health(const float OldHealth)
{
	const float Damage = Health - OldHealth;

	OnHealthChanged.Broadcast(this, Health, Damage, nullptr, nullptr, nullptr);
}

void USAttributeComponent::HandleTakeAnyDamage(AActor* DamagedActor,const float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f || bIsDead)
	{
		return;
	}

	// Update health clamped
	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(Health));

	bIsDead = Health <= 0.0f;

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);
	
	/*if (ASGameModeWave* GM = Cast<ASGameModeWave>(GetWorld()->GetAuthGameMode()))
	{
		//ASPlayerState* PS = Cast<ASPlayerState>(InstigatedBy->PlayerState);
		//GM->OnActorKilled.Broadcast(nullptr, nullptr, nullptr, PS);
	}*/

	if (!bIsDead)
	{
		LaunchTimerRegen();
	}

	if (bIsDead)
	{
		GetWorld()->GetTimerManager().ClearTimer(RegenTimerHandle);
		
		switch (ControllerType)
		{
			default: break;

			case EControllerPossType::Player:
			break;
		}
		
		// Access BP
		Dead(ControllerType);
	}

}

void USAttributeComponent::LaunchTimerRegen()
{
	GetWorld()->GetTimerManager().SetTimer(RegenTimerHandle, this, &USAttributeComponent::RegenerationHealth, DelayRegen, true);
}

void USAttributeComponent::RegenerationHealth()
{

	Health = FMath::Clamp(Health + RegenHealth, 0.0f, DefaultHealth);

	OnHealthChanged.Broadcast(this, Health, 0.0f, nullptr, nullptr, nullptr);

	if (Health == DefaultHealth)
	{
		GetWorld()->GetTimerManager().ClearTimer(RegenTimerHandle);
	}
}

void USAttributeComponent::ResetHealth()
{
	Health = DefaultHealth;
	bIsDead = false;

	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(Health));
}

void USAttributeComponent::Heal(const float HealAmount)
{
	if (HealAmount <= 0.0f || Health <= 0.0f)
	{
		return;
	}

	Health = FMath::Clamp(Health + HealAmount, 0.0f, DefaultHealth);

	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s (+%s)"), *FString::SanitizeFloat(Health), *FString::SanitizeFloat(HealAmount));

	OnHealthChanged.Broadcast(this, Health, -HealAmount, nullptr, nullptr, nullptr);

}

void USAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USAttributeComponent, Health);
}