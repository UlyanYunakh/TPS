// Ullian Yunakh.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponPickUp.generated.h"

UCLASS()
class TPS_API AWeaponPickUp : public AActor
{
	GENERATED_BODY()
	
public:
	AWeaponPickUp();

public:
	virtual void Destroyed() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION(Server, Reliable)
	void TriggerPickUp(AActor* OtherActor);
	void TriggerPickUp_Implementation(AActor* OtherActor);

private:
	UFUNCTION(Server, Reliable)
	void UpdateWeaponPreview();
	void UpdateWeaponPreview_Implementation();

public:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category="Weapon")
	TSubclassOf<class AAutoWeapon> WeaponClass;

protected:
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAcces = "true"))
	class USphereComponent* PickupTrigger;

private:
	UPROPERTY(Replicated)
	class AAutoWeapon* PreviewWeapon;
};
