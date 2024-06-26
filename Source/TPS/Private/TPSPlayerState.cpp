// Ullian Yunakh.


#include "TPSPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "TPS/Public/TPSPlayerController.h"


void ATPSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATPSPlayerState, bWaitingForMatchStart);
	DOREPLIFETIME(ATPSPlayerState, Kills);
	DOREPLIFETIME(ATPSPlayerState, Deaths);
}


void ATPSPlayerState::SetWaitingForMatchStart_Implementation(bool NewValue)
{
	bWaitingForMatchStart = NewValue;
	OnRep_WaitingForMatchStart(); // for listen server
}


void ATPSPlayerState::OtherPlayerKilled_Implementation(APawn* OtherPlayer)
{
	Kills++;
}

void ATPSPlayerState::KilledByOtherPlayer_Implementation(APawn* OtherPlayer)
{
	Deaths++;
}


void ATPSPlayerState::SetRespawnTimer_Implementation(float RespawnTime)
{
	if (GetWorld())
	{
		GetWorldTimerManager().SetTimer(RespawnTimer, RespawnTime, false);
	}
}


void ATPSPlayerState::OnRep_WaitingForMatchStart()
{
	if (ATPSPlayerController* TPS_PC = Cast<ATPSPlayerController>(GetPlayerController()))
	{
		if (bWaitingForMatchStart)
		{
			TPS_PC->DisableInput(TPS_PC);
			TPS_PC->GetPawnOrSpectator()->DisableInput(TPS_PC);
		}
		else
		{
			TPS_PC->EnableInput(TPS_PC);
			TPS_PC->GetPawnOrSpectator()->EnableInput(TPS_PC);
		}
	}
}