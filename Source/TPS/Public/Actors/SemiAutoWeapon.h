// Ullian Yunakh.

#pragma once

#include "CoreMinimal.h"
#include "AutoWeapon.h"
#include "SemiAutoWeapon.generated.h"

UCLASS()
class TPS_API ASemiAutoWeapon : public AAutoWeapon
{
	GENERATED_BODY()
	
public:
	ASemiAutoWeapon();

protected:
	virtual void FireOnce_Implementation() override;
	virtual void StopFiring_Implementation() override;

private:
	bool bFiredOnce;
};
