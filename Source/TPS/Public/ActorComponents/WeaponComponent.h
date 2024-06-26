// Ullian Yunakh.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPS_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWeaponComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Weapon")
	virtual void InitNewWeapon(TSubclassOf<class AAutoWeapon> WeaponClass);
	virtual void InitNewWeapon_Implementation(TSubclassOf<class AAutoWeapon> WeaponClass);

	UFUNCTION(Server, Unreliable)
	virtual void UpdateWeaponAimDirection(const FVector& CameraLocation);
	virtual void UpdateWeaponAimDirection_Implementation(const FVector& CameraLocation);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	class AAutoWeapon* GetCurrWeapon() const { return CurrWeapon; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AAutoWeapon> StartingWeaponClass;

private:
	UPROPERTY(Replicated)
	class AAutoWeapon* CurrWeapon;
};
