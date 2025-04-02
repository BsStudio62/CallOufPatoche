// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameStateWave.h"
#include "SPlayerController.h"
#include "Widget/SHud.h"
// Access Macro Multiplayer
#include "Net/UnrealNetwork.h"

void ASGameStateWave::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASGameStateWave, Wave);

}

void ASGameStateWave::OnRep_Wave()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		ASPlayerController* PC = Cast<ASPlayerController>(It->Get());
		if (PC && PC->IsLocalController())
		{
			USHud* Hud = Cast<USHud>(PC->GetHud());
			
			if (Hud)
			{
				Hud->UpdateHud(EUpdateHud::Wave, true, nullptr);
			}
		}
	}

}

void ASGameStateWave::IncrementeWave()
{
	
	Wave++;

	// Call In Server
	OnRep_Wave();
}
