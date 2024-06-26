// Ullian Yunakh.


#include "ActorComponents/HealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "TPSCharacter.h"
#include "TPSPlayerState.h"


UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bAutoActivate = true;
	SetIsReplicatedByDefault(true);
}


void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, CurrHealth);
}


void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority)
	{
		CurrHealth = MaxHealth;

		if (GetOwner())
		{
			GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::OnDamageTaken);
		}
	}
}


void UHealthComponent::OnDamageTaken_Implementation(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	CurrHealth = FMath::Clamp(CurrHealth - Damage, 0.f, MaxHealth);

	if (CurrHealth <= 0)
	{
		GetOwner()->OnTakeAnyDamage.RemoveDynamic(this, &UHealthComponent::OnDamageTaken);

		if (DamageCauser->GetInstigator() && GetOwner()->GetInstigator())
		{
			if (ATPSPlayerState* TPS_PS_Causer = DamageCauser->GetInstigator()->GetPlayerState<ATPSPlayerState>())
			{
				TPS_PS_Causer->OtherPlayerKilled(GetOwner()->GetInstigator());
			}

			if (ATPSPlayerState* TPS_PS_Damaged = GetOwner()->GetInstigator()->GetPlayerState<ATPSPlayerState>())
			{
				TPS_PS_Damaged->KilledByOtherPlayer(DamageCauser->GetInstigator());
			}
		}

		if (ATPSCharacter* character = GetOwner<ATPSCharacter>())
		{
			character->CharacterDeath();
		}
	}
}


void UHealthComponent::AddHealth_Implementation(const float Value)
{
	
}

