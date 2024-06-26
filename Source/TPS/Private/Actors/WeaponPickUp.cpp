// Ullian Yunakh.


#include "Actors/WeaponPickUp.h"
#include "Net/UnrealNetwork.h"
#include "Components/SphereComponent.h"
#include "Actors/AutoWeapon.h"
#include "ActorComponents/WeaponComponent.h"

AWeaponPickUp::AWeaponPickUp()
{
	bReplicates = true;
 	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	PickupTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("PickUpTrigger"));
	PickupTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	PickupTrigger->SetSphereRadius(50.0f);
	PickupTrigger->SetRelativeLocation(FVector(0.0f, 0.0f, 80.0f));
	PickupTrigger->SetupAttachment(RootComponent);
}


void AWeaponPickUp::Destroyed()
{
	Super::Destroyed();

	if (PreviewWeapon)
	{
		PreviewWeapon->Destroy();
	}
}

void AWeaponPickUp::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!PreviewWeapon || PreviewWeapon->GetClass() != WeaponClass)
	{
		UpdateWeaponPreview();
	}
}


void AWeaponPickUp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponPickUp, WeaponClass);
	DOREPLIFETIME(AWeaponPickUp, PickupTrigger);
	DOREPLIFETIME(AWeaponPickUp, PreviewWeapon);
}


void AWeaponPickUp::UpdateWeaponPreview_Implementation()
{
	if (UWorld* world = GetWorld())
	{
		if (PreviewWeapon)
		{
			PreviewWeapon->Destroy();
		}

		PreviewWeapon = world->SpawnActor<AAutoWeapon>(WeaponClass);
		if (PreviewWeapon)
		{
			PreviewWeapon->AttachToComponent(PickupTrigger, FAttachmentTransformRules::SnapToTargetIncludingScale);
		}
	}
}


void AWeaponPickUp::TriggerPickUp_Implementation(AActor* OtherActor)
{
	if (UWeaponComponent* weaponComp = OtherActor->GetComponentByClass<UWeaponComponent>())
	{
		UClass* newWeaponClass = weaponComp->GetCurrWeapon()->GetClass();
		weaponComp->InitNewWeapon(WeaponClass);
		WeaponClass = newWeaponClass;

		if (!PreviewWeapon || PreviewWeapon->GetClass() != WeaponClass)
		{
			UpdateWeaponPreview();
		}
	}
}

