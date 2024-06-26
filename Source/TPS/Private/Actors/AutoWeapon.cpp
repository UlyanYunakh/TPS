// Ullian Yunakh.


#include "Actors/AutoWeapon.h"
#include <Actors/Projectile.h>
#include "Components/ArrowComponent.h"
#include "Net/UnrealNetwork.h"


AAutoWeapon::AAutoWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	HandleArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("Handle location"));
	HandleArrowComp->SetupAttachment(RootComponent);

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon skeleton mesh"));
	SkeletalMesh->SetVisibility(false);
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SkeletalMesh->SetupAttachment(RootComponent);
	
	BarrelArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("Barrel location"));
	BarrelArrowComp->SetRelativeLocation(FVector(100.0f, 0.0f, 0.0f));
	BarrelArrowComp->SetupAttachment(SkeletalMesh);
}


void AAutoWeapon::OnConstruction(const FTransform& Transform)
{
	TArray<USceneComponent*> childrenComps;
	SkeletalMesh->GetChildrenComponents(true, childrenComps);
	for (USceneComponent* comp : childrenComps)
	{
		if (UPrimitiveComponent* primitiveComp = Cast<UPrimitiveComponent>(comp))
		{
			primitiveComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}


void AAutoWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAutoWeapon, CurrCapacity);
	DOREPLIFETIME(AAutoWeapon, WeaponState);
	DOREPLIFETIME(AAutoWeapon, ReloadTimer);	
	DOREPLIFETIME(AAutoWeapon, AimState);
	DOREPLIFETIME(AAutoWeapon, BarrelLocation);
	DOREPLIFETIME(AAutoWeapon, ProjectileTransform);
}


void AAutoWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetLocalRole() == ROLE_Authority)
	{
		bReadyToFire = true;
		TimeFromLastShot = 0;
		CurrCapacity = Capacity;

		StopAimDownsight();
	}
}


void AAutoWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// update barrel location on the server (use attach parent as source of truth)
	if (GetAttachParentActor() && GetAttachParentActor()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		SetBarrelLocation(BarrelArrowComp->GetComponentLocation());
	}

	if (GetLocalRole() == ROLE_Authority)
	{
		FiringTick(DeltaTime);
	}
}


void AAutoWeapon::FiringTick_Implementation(const float DeltaTime)
{
	if (WeaponState != EWeaponState::Reloading && !bReadyToFire)
	{
		TimeFromLastShot += DeltaTime;
		bReadyToFire = TimeFromLastShot >= FiringRate;
	}

	if (WeaponState == EWeaponState::Firing && bReadyToFire)
	{
		FireOnce();
		bReadyToFire = false;
		TimeFromLastShot = 0;
	}
}


void AAutoWeapon::StartAimDownsight_Implementation()
{
	AimState = EWeaponAimState::Downsight;
	CurrSpreadAngle = FMath::DegreesToRadians(MaxDownsightSpread);
}


void AAutoWeapon::StopAimDownsight_Implementation()
{
	AimState = EWeaponAimState::Hip;
	CurrSpreadAngle = FMath::DegreesToRadians(MaxHipSpread);
}


void AAutoWeapon::SetBarrelLocation_Implementation(FVector NewBarrelLocation)
{
	BarrelLocation = NewBarrelLocation;
}


void AAutoWeapon::OnRep_ProjectileTransform()
{
	SpawnShootParticles(ProjectileTransform);
}


float AAutoWeapon::GetCurrentMaxSpread()
{
	return AimState == EWeaponAimState::Downsight ? MaxDownsightSpread : MaxHipSpread;
}


void AAutoWeapon::StartFiring_Implementation()
{
	// do not update state if current state is reloading 
	if (WeaponState != EWeaponState::Reloading)
	{
		WeaponState = EWeaponState::Firing;
	}
}


void AAutoWeapon::StopFiring_Implementation()
{
	// do not update state if current state is reloading 
	if (WeaponState != EWeaponState::Reloading) 
	{
		WeaponState = EWeaponState::NotFiring;
	}
}


void AAutoWeapon::StartReload_Implementation()
{
	if (ReloadTimer.IsValid())
	{
		GetWorldTimerManager().ClearTimer(ReloadTimer);
	}

	WeaponState = EWeaponState::Reloading;
	GetWorldTimerManager().SetTimer(ReloadTimer, this, &AAutoWeapon::Reload, ReloadTime, false);
}


void AAutoWeapon::Reload_Implementation()
{
	CurrCapacity = Capacity;
	WeaponState = EWeaponState::NotFiring;
	bReadyToFire = true;
	GetWorldTimerManager().ClearTimer(ReloadTimer);
}


void AAutoWeapon::FireOnce_Implementation()
{
	FVector direction = FMath::VRandCone(AimDirection, CurrSpreadAngle);
	FQuat spawnRotator = direction.Rotation().Quaternion();

	FVector barrelLocation;
	GetBarrelLocation(barrelLocation);
	
	FTransform spawnTransform;
	spawnTransform.SetLocation(barrelLocation);
	spawnTransform.SetRotation(spawnRotator);

	FActorSpawnParameters spawnParams;
	spawnParams.Instigator = GetInstigator();
	spawnParams.Owner = this;

	GetWorld()->SpawnActor<AProjectile>(ProjectileClass, spawnTransform, spawnParams);

	ProjectileTransform = spawnTransform;
	OnRep_ProjectileTransform(); // for listen server

	CurrCapacity--;
	if (CurrCapacity <= 0)
	{
		StartReload();
	}
}


