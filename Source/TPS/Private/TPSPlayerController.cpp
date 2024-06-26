// Ullian Yunakh.


#include "TPSPlayerController.h"
#include "TPSGameMode.h"
#include "Net/UnrealNetwork.h"


void ATPSPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	OnPlayerStateReplication.Broadcast(PlayerState);
}


void ATPSPlayerController::OnUnPossess()
{
	Super::OnUnPossess();
	
	ATPSGameMode* TPS_GM = GetWorld()->GetAuthGameMode<ATPSGameMode>();
	if (TPS_GM)
	{
		StartSpot.Reset(); // set StartSpot to nullptr to select random starting point when respawning;
		TPS_GM->RestartRequest(this);
	}
}


void ATPSPlayerController::InstigatedAnyDamage(float Damage, const UDamageType* DamageType, AActor* DamagedActor, AActor* DamageCauser)
{
	Super::InstigatedAnyDamage(Damage, DamageType, DamagedActor, DamageCauser);

	if (APawn* pawn = Cast<APawn>(DamagedActor))
	{
		TriggerHitMark();
	}
}




void ATPSPlayerController::TriggerHitMark_Implementation()
{
	OnHitMark.Broadcast();
}
