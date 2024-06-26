// Ullian Yunakh.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TPSPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHitMark);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStateReplication, APlayerState*, NewPlayerState);

UCLASS()
class TPS_API ATPSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void OnRep_PlayerState() override;

protected:
	virtual void OnUnPossess() override;
	virtual void InstigatedAnyDamage(float Damage, const class UDamageType* DamageType, class AActor* DamagedActor, class AActor* DamageCauser);

public:
	UFUNCTION(Client, Unreliable)
	virtual void TriggerHitMark();
	virtual void TriggerHitMark_Implementation();

protected:
	/* for cosmetic purpose only */
	UPROPERTY(BlueprintAssignable, meta = (AllowPrivateAccess = "true"))
	FOnHitMark OnHitMark;

	UPROPERTY(BlueprintAssignable, meta = (AllowPrivateAccess = "true"))
	FOnPlayerStateReplication OnPlayerStateReplication;
};
