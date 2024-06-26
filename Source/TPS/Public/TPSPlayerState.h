// Ullian Yunakh.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TPSPlayerState.generated.h"


UCLASS()
class TPS_API ATPSPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION(Server, Reliable)
	virtual void SetWaitingForMatchStart(bool NewValue);
	virtual void SetWaitingForMatchStart_Implementation(bool NewValue);

	UFUNCTION(Server, Reliable)
	virtual void OtherPlayerKilled(APawn* OtherPlayer);
	virtual void OtherPlayerKilled_Implementation(APawn* OtherPlayer);

	UFUNCTION(Server, Reliable)
	virtual void KilledByOtherPlayer(APawn* OtherPlayer);
	virtual void KilledByOtherPlayer_Implementation(APawn* OtherPlayer);

public:
	UFUNCTION(Client, Reliable)
	void SetRespawnTimer(float RespawnTime);
	void SetRespawnTimer_Implementation(float RespawnTime);

protected:
	UFUNCTION()
	virtual void OnRep_WaitingForMatchStart();

public:
	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float Kills;

	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float Deaths;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_WaitingForMatchStart, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bWaitingForMatchStart;

protected:
	/* For cosmetic purpose only */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTimerHandle RespawnTimer;
};

