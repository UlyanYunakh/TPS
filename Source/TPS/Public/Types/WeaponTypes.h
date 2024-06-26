// Ullian Yunakh.

#pragma once

#include "CoreMinimal.h"
#include "WeaponTypes.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	None = 0,
	NotFiring UMETA(DisplayName="Not firing"),
	Firing UMETA(DisplayName = "Firing"),
	Reloading UMETA(DisplayName = "Reloading")
};

UENUM(BlueprintType)
enum class EWeaponAimState : uint8
{
	None = 0,
	Hip UMETA(DisplayName="Hip"),
	Downsight UMETA(DisplayName="Downsight")
};

UENUM(BlueprintType)
enum class EWeaponHandlingType : uint8
{
	None = 0,
	Rifle UMETA(DisplayName = "Rifle, two-handed"),
	Pistol UMETA(DisplayName = "Pistol, one-handed")
};