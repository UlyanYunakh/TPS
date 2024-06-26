// Ullian Yunakh.


#include "Actors/SemiAutoWeapon.h"


ASemiAutoWeapon::ASemiAutoWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
}


void ASemiAutoWeapon::FireOnce_Implementation()
{
	if (!bFiredOnce)
	{
		Super::FireOnce_Implementation();
		bFiredOnce = true;
	}
}


void ASemiAutoWeapon::StopFiring_Implementation()
{
	Super::StopFiring_Implementation();
	bFiredOnce = false;
}


