// Ullian Yunakh.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TPSGameStateBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMatchStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMatchEnded);


UCLASS()
class TPS_API ATPSGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	ATPSGameStateBase();

public:
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

public:
	UFUNCTION(Server, Reliable)
	virtual void EndMatch();
	virtual void EndMatch_Implementation();

	UFUNCTION(Server, Reliable)
	virtual void SetWaitingForMatchStart(bool NewValue);
	virtual void SetWaitingForMatchStart_Implementation(bool NewValue);

	UFUNCTION(Server, Reliable)
	virtual void StartMatchCountdown();
	virtual void StartMatchCountdown_Implementation();

protected:
	UFUNCTION(Server, Reliable)
	virtual void StartMatchTimer();
	virtual void StartMatchTimer_Implementation();

public:
	UFUNCTION(BlueprintCallable)
	TArray<class ATPSPlayerState*> GetSortedPlayerStateArray();

private:
	void UpdateRemainingTime();

private:
	UFUNCTION()
	void OnRep_MatchInProgress();

	UFUNCTION()
	void OnRep_MatchEnded();

protected:
	UPROPERTY(BlueprintAssignable, meta = (AllowPrivateAccess = "true"))
	FOnMatchStarted OnMatchStarted;

	UPROPERTY(BlueprintAssignable, meta = (AllowPrivateAccess = "true"))
	FOnMatchEnded OnMatchEnded;

protected:
	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MatchCountdownRemainingTime;

	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MathRemainingTime;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MatchStartCountdown = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MatchDuration = 300.0f;

private:
	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bWaitingForMatchStart;

	UPROPERTY(ReplicatedUsing = OnRep_MatchInProgress, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bMatchInProgress;

	UPROPERTY(ReplicatedUsing = OnRep_MatchEnded, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bMatchEnded;

private:
	FTimerHandle MatchCountdownTimer;
	FTimerHandle MatchTimer;
};
