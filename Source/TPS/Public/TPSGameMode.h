// Ullian Yunakh.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TPSGameMode.generated.h"

UCLASS(minimalapi)
class ATPSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATPSGameMode();

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;

public:
	UFUNCTION(Server, Reliable, BlueprintCallable)
	virtual void RestartRequest(ATPSPlayerController* Player, float RespawnTime = -1);
	virtual void RestartRequest_Implementation(ATPSPlayerController* Player, float RespawnTime = -1);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float DefaultRespawnTime = 5.0f;
};



