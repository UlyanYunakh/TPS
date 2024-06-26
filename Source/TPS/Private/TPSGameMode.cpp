// Ullian Yunakh.

#include "TPSGameMode.h"
#include "TPSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "TPS/Public/TPSPlayerController.h"
#include "TPS/Public/TPSPlayerState.h"
#include <TPSGameStateBase.h>

ATPSGameMode::ATPSGameMode()
{

}


void ATPSGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ATPSGameStateBase* TPS_GS = GetGameState<ATPSGameStateBase>();
	if (TPS_GS)
	{
		TPS_GS->SetWaitingForMatchStart(true);

		if (GetNumPlayers() > 1) // match can be started if more than 1 player connected
		{
			TPS_GS->StartMatchCountdown();
		}
	}
}


void ATPSGameMode::RestartRequest_Implementation(ATPSPlayerController* Player, float RespawnTime /* = -1 */)
{	
	FTimerHandle respawnTimer;

	FTimerDelegate delegate;
	delegate.BindUObject(this, &ATPSGameMode::RestartPlayer, (AController*)Player);

	RespawnTime = RespawnTime < 0 ? DefaultRespawnTime : RespawnTime;

	GetWorldTimerManager().SetTimer(respawnTimer, delegate, RespawnTime, false);

	if (ATPSPlayerState* TPS_PS = Player->GetPlayerState<ATPSPlayerState>())
	{
		TPS_PS->SetRespawnTimer(RespawnTime); // start respawn timer on client (for cosmetic purposes)
	}
}
