// Ullian Yunakh.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DELEGATE(FOnDeath);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPS_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Health Component")
	virtual void AddHealth(const float Value);

protected:
	UFUNCTION()
	virtual void OnRep_CurrHealth() const;

	UFUNCTION()
	virtual void OnDamageTaken(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:
	FOnDeath OnDeath;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Health Component", meta = (AllowPrivateAccess = "true"))
	float MaxHealth = 100.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Health Component", ReplicatedUsing= OnRep_CurrHealth)
	float CurrHealth;
		
};
