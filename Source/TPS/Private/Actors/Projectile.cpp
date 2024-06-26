// Ullian Yunakh.


#include "Actors/Projectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "TPSPlayerState.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"


AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SetLifeSpan(10.0f);

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->InitSphereRadius(25.0f);
	//Sphere->SetUseCCD(true);
	Sphere->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	Sphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	RootComponent = Sphere;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	Mesh->CastShadow = false;
	Mesh->SetupAttachment(Sphere);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->SetUpdatedComponent(Sphere);
	ProjectileMovement->InitialSpeed = 10000.0f;
	ProjectileMovement->MaxSpeed = 10000.0f;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
}


void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		if (ACharacter* character = GetInstigator<ACharacter>())
		{
			character->GetCapsuleComponent()->IgnoreActorWhenMoving(this, true);
			Sphere->IgnoreActorWhenMoving(character, true);
		}

		Sphere->OnComponentHit.AddDynamic(this, &AProjectile::ProjectileHit);
	}
}

void AProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AProjectile, HitResult);
}


void AProjectile::ProjectileHit_Implementation(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Sphere->OnComponentHit.RemoveDynamic(this, &AProjectile::ProjectileHit);

	UGameplayStatics::ApplyPointDamage(OtherActor, Damage, NormalImpulse, Hit, GetInstigator()->GetController(), this, DamageType);
	
	HitResult = Hit;
	OnRep_HitResult(); // for listen server

	SetActorHiddenInGame(true);
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(5.0f);
}


void AProjectile::OnRep_HitResult()
{
	SpawnHitParticles(HitResult);

	SetActorHiddenInGame(true);
}

