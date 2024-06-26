// Ullian Yunakh.


#include "TPSGameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "TPSGameMode.h"
#include "TPSPlayerState.h"


ATPSGameStateBase::ATPSGameStateBase()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickInterval(0.1f);
	SetActorTickEnabled(true);
}


void ATPSGameStateBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetLocalRole() == ROLE_Authority)
	{
		UpdateRemainingTime();
	}
}


void ATPSGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATPSGameStateBase, bWaitingForMatchStart);
	DOREPLIFETIME(ATPSGameStateBase, bMatchInProgress);
	DOREPLIFETIME(ATPSGameStateBase, bMatchEnded);

	DOREPLIFETIME(ATPSGameStateBase, MatchCountdownRemainingTime);
	DOREPLIFETIME(ATPSGameStateBase, MathRemainingTime);
}


void ATPSGameStateBase::SetWaitingForMatchStart_Implementation(bool NewValue)
{
	bWaitingForMatchStart = NewValue;

	for (APlayerState* PS : PlayerArray)
	{
		if (ATPSPlayerState* TPS_PS = Cast<ATPSPlayerState>(PS))
		{
			TPS_PS->SetWaitingForMatchStart(bWaitingForMatchStart);
		}
	}
}


void ATPSGameStateBase::StartMatchCountdown_Implementation()
{
	GetWorldTimerManager().SetTimer(MatchCountdownTimer, this, &ATPSGameStateBase::StartMatchTimer, MatchStartCountdown, false);
}


void ATPSGameStateBase::StartMatchTimer_Implementation()
{
	GetWorldTimerManager().ClearTimer(MatchCountdownTimer);
	MatchCountdownRemainingTime = 0;

	SetWaitingForMatchStart(false);
	GetWorldTimerManager().SetTimer(MatchTimer, this, &ATPSGameStateBase::EndMatch, MatchDuration, false);	
	bMatchInProgress = true;
	OnRep_MatchInProgress(); // for listen server
}


TArray<class ATPSPlayerState*> ATPSGameStateBase::GetSortedPlayerStateArray()
{
	TArray<ATPSPlayerState*> OutArray;

	for (APlayerState* PS : PlayerArray)
	{
		if (ATPSPlayerState* ATPS_PS = Cast<ATPSPlayerState>(PS))
		{
			OutArray.Add(ATPS_PS);
		}
	}

	OutArray.Sort([](const ATPSPlayerState& A, const ATPSPlayerState& B) { return A.Kills - A.Deaths > B.Kills - B.Deaths; });

	return OutArray;
}


void ATPSGameStateBase::UpdateRemainingTime()
{
	if (MatchCountdownTimer.IsValid())
	{
		MatchCountdownRemainingTime = GetWorldTimerManager().GetTimerRemaining(MatchCountdownTimer);
	}

	if (MatchTimer.IsValid())
	{
		MathRemainingTime = GetWorldTimerManager().GetTimerRemaining(MatchTimer);
	}
}


void ATPSGameStateBase::OnRep_MatchInProgress()
{
	if (bMatchInProgress)
	{
		OnMatchStarted.Broadcast();
	}
}

void ATPSGameStateBase::OnRep_MatchEnded()
{
	if (bMatchEnded)
	{
		OnMatchEnded.Broadcast();
	}
}


void ATPSGameStateBase::EndMatch_Implementation()
{
	GetWorldTimerManager().ClearTimer(MatchTimer);
	MathRemainingTime = 0;
	bMatchEnded = true;
	OnRep_MatchEnded(); // for listen server

	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, TEXT("Mathc ended"));
}
