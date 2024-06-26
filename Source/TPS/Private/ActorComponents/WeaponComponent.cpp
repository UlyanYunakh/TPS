// Ullian Yunakh.


#include "ActorComponents/WeaponComponent.h"
#include "Net/UnrealNetwork.h"
#include "Actors/AutoWeapon.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"


UWeaponComponent::UWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bAutoActivate = true;
	SetIsReplicatedByDefault(true);
}


void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority)
	{
		InitNewWeapon(StartingWeaponClass);
	}
}


void UWeaponComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	if (CurrWeapon)
	{
		CurrWeapon->Destroy(); // TODO: Implement drop
	}
}


void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponComponent, CurrWeapon);
}


void UWeaponComponent::UpdateWeaponAimDirection_Implementation(const FVector& CameraLocation)
{
	if (!CurrWeapon) return;

	FHitResult hitResult;

	FRotator cameraRotator = GetOwner()->GetInstigator()->GetControlRotation();
	FVector aimDirectiom = UKismetMathLibrary::GetForwardVector(cameraRotator);
	FVector distantCameraLocation = CameraLocation + aimDirectiom * 20000.0f;

	GetWorld()->LineTraceSingleByChannel(hitResult, CameraLocation, distantCameraLocation, ECollisionChannel::ECC_Visibility);

	if (hitResult.bBlockingHit)
	{
		FVector barrelLocation;
		CurrWeapon->GetBarrelLocation(barrelLocation);

		FRotator lookAtTargetRotator = UKismetMathLibrary::FindLookAtRotation(barrelLocation, hitResult.Location);
		aimDirectiom = UKismetMathLibrary::GetForwardVector(lookAtTargetRotator);
	}

	CurrWeapon->SetAimDirection(aimDirectiom);
}


void UWeaponComponent::InitNewWeapon_Implementation(TSubclassOf<class AAutoWeapon> WeaponClass)
{
	if (CurrWeapon)
	{
		CurrWeapon->Destroy(); // TODO: implement weapon drop
	}

	FTransform spawnTrafsform;
	FActorSpawnParameters spawnParams;
	spawnParams.Instigator = GetOwner()->GetInstigator();
	spawnParams.Owner = GetOwner();

	AAutoWeapon* spawnedWeapon = GetWorld()->SpawnActor<AAutoWeapon>(WeaponClass, spawnTrafsform, spawnParams);
	if (spawnedWeapon)
	{
		CurrWeapon = spawnedWeapon;

		USceneComponent* ownerRoot = GetOwner<ACharacter>()->GetMesh();
		FAttachmentTransformRules attachRule = FAttachmentTransformRules::SnapToTargetIncludingScale;
		FName socketName = FName(TEXT("hand_r_socket"));

		CurrWeapon->AttachToComponent(ownerRoot, attachRule, socketName);
	}
}
