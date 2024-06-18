// Ullian Yunakh.


#include "ActorComponents/HealthComponent.h"
#include "Net/UnrealNetwork.h"


UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bAutoActivate = true;

	CurrHealth = MaxHealth;
}

void UHealthComponent::BeginPlay()
{
	if (auto owner = GetOwner())
	{
		owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::OnDamageTaken);
	}
}


void UHealthComponent::OnDamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	AddHealth(-Damage);
}


void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, CurrHealth);
}


void UHealthComponent::AddHealth(const float Value)
{
	if (GetOwner() && GetOwner()->GetLocalRole() == ROLE_Authority)
	{
		CurrHealth = FMath::Clamp(CurrHealth + Value, 0.f, MaxHealth);
	}
}


void UHealthComponent::OnRep_CurrHealth() const
{
	if (CurrHealth <= 0)
	{
		if (auto owner = GetOwner())
		{
			owner->OnTakeAnyDamage.RemoveDynamic(this, &UHealthComponent::OnDamageTaken);
		}

		OnDeath.ExecuteIfBound();
	}
}

