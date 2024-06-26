// Ullian Yunakh.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Types/WeaponTypes.h>
#include "AutoWeapon.generated.h"

UCLASS()
class TPS_API AAutoWeapon : public AActor
{
	GENERATED_BODY()
	
public:
	AAutoWeapon();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION(Server, Reliable, BlueprintCallable)
	virtual void StartFiring();
	virtual void StartFiring_Implementation();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	virtual void StopFiring();
	virtual void StopFiring_Implementation();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	virtual void StartReload();
	virtual void StartReload_Implementation();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	virtual void StartAimDownsight();
	virtual void StartAimDownsight_Implementation();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	virtual void StopAimDownsight();
	virtual void StopAimDownsight_Implementation();

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsFiring() { return WeaponState == EWeaponState::Firing; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsAbleToFire() { return WeaponState != EWeaponState::Reloading; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void GetBarrelLocation(FVector& OutLocation) { OutLocation = BarrelLocation; }

	FORCEINLINE void SetAimDirection(const FVector& NewAimDirection) { AimDirection = NewAimDirection; }

protected:
	UFUNCTION(Server, Unreliable)
	virtual void FiringTick(const float DeltaTime);
	virtual void FiringTick_Implementation(const float DeltaTime);

	UFUNCTION(Server, Unreliable)
	virtual void FireOnce();
	virtual void FireOnce_Implementation();

	UFUNCTION(Server, Reliable)
	virtual void Reload();
	virtual void Reload_Implementation();

	UFUNCTION(Server, Unreliable)
	virtual void SetBarrelLocation(FVector NewBarrelLocation);
	virtual void SetBarrelLocation_Implementation(FVector NewBarrelLocation);

	UFUNCTION()
	void OnRep_ProjectileTransform();

	UFUNCTION(BlueprintNativeEvent)
	void SpawnShootParticles(const FTransform& Transform) const;
	FORCEINLINE void SpawnShootParticles_Implementation(const FTransform& Transform) const { }

private:
	float GetCurrentMaxSpread();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	float FiringRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	float MaxHipSpread;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	float MaxDownsightSpread;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	FString WeaponName = TEXT("Weapon");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	int64 Capacity = 20;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	float ReloadTime = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	EWeaponHandlingType HandlingType;

protected:
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	EWeaponState WeaponState;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	int64 CurrCapacity;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	EWeaponAimState AimState;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	FTimerHandle ReloadTimer;

	UPROPERTY(ReplicatedUsing = OnRep_ProjectileTransform, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	FTransform ProjectileTransform;

	/* Server barrel location */
	UPROPERTY(Replicated)
	FVector BarrelLocation;

private:
	bool bReadyToFire;
	float TimeFromLastShot;
	FVector AimDirection;
	float CurrSpreadAngle;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* BarrelArrowComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* HandleArrowComp;
};
