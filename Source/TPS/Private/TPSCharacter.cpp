// Ullian Yunakh.

#include "TPSCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

#include "ActorComponents/HealthComponent.h"
#include "ActorComponents/WeaponComponent.h"
#include "Actors/WeaponPickUp.h"
#include "Actors/AutoWeapon.h"
#include "TPSGameMode.h"
#include "Net/UnrealNetwork.h"


ATPSCharacter::ATPSCharacter()
{
	bReplicates = true;

	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 1200.f;
	GetCharacterMovement()->AirControl = 0.5f;
	GetCharacterMovement()->GravityScale = 1.0f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 200.0f; 
	CameraBoom->SocketOffset = FVector(0.0f, 80.0f, 80.0f);
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
		
	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	HealthComp->SetIsReplicated(true);

	WeaponComp = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComponent"));
	WeaponComp->SetIsReplicated(true);
}

void ATPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATPSCharacter, bAimingDownsight);
	DOREPLIFETIME(ATPSCharacter, bDead);
}

void ATPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}


void ATPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATPSCharacter::Move);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATPSCharacter::Look);

		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ATPSCharacter::FireTriggered);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ATPSCharacter::FireCompleted);

		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &ATPSCharacter::AimTriggered);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ATPSCharacter::AimCompleted);

		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &ATPSCharacter::ReloadTriggered);

		EnhancedInputComponent->BindAction(PickUpAction, ETriggerEvent::Started, this, &ATPSCharacter::PickUpStarted);
	}
}

void ATPSCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ATPSCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}


void ATPSCharacter::FireTriggered(const FInputActionValue& Value)
{
	if (!WeaponComp || !WeaponComp->GetCurrWeapon()) return;

	if (WeaponComp->GetCurrWeapon()->IsAbleToFire() && !WeaponComp->GetCurrWeapon()->IsFiring())
	{
		WeaponComp->GetCurrWeapon()->StartFiring();
	}
}


void ATPSCharacter::FireCompleted(const FInputActionValue& Value)
{
	if (!WeaponComp || !WeaponComp->GetCurrWeapon()) return;

	WeaponComp->GetCurrWeapon()->StopFiring();
}


void ATPSCharacter::AimTriggered(const FInputActionValue& Value)
{
	if (WeaponComp && WeaponComp->GetCurrWeapon())
	{
		WeaponComp->GetCurrWeapon()->StartAimDownsight();

		SetAimingDownsight(true);

		CameraBoom->TargetArmLength = 100.0f;
		FollowCamera->FieldOfView = 70.0f;
		GetCharacterMovement()->MaxWalkSpeed = 200.f;
	}
}


void ATPSCharacter::AimCompleted(const FInputActionValue& Value)
{
	if (WeaponComp && WeaponComp->GetCurrWeapon())
	{
		WeaponComp->GetCurrWeapon()->StopAimDownsight();

		SetAimingDownsight(false);

		CameraBoom->TargetArmLength = 200.0f;
		FollowCamera->FieldOfView = 90.0f;
		GetCharacterMovement()->MaxWalkSpeed = 500.f;
	}
}


void ATPSCharacter::ReloadTriggered(const FInputActionValue& Value)
{
	if (WeaponComp && WeaponComp->GetCurrWeapon())
	{
		WeaponComp->GetCurrWeapon()->StartReload();
	}
}


void ATPSCharacter::PickUpStarted(const FInputActionValue& Value)
{
	PickUp();
}

void ATPSCharacter::OnRep_Dead()
{
	if (bDead)
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCharacterMovement()->DisableMovement();
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		GetMesh()->SetAllBodiesSimulatePhysics(true);
	}
}


void ATPSCharacter::PickUp_Implementation()
{
	TArray<AActor*> overlappingActors;
	GetOverlappingActors(overlappingActors, AWeaponPickUp::StaticClass());

	// trigger only one pickup
	if (overlappingActors.IsValidIndex(0) && overlappingActors[0])
	{
		AWeaponPickUp* pickUp = (AWeaponPickUp*)overlappingActors[0];
		pickUp->TriggerPickUp(this);
	}
}


void ATPSCharacter::CharacterDeath_Implementation()
{
	if (AController* controller = GetController())
	{
		controller->UnPossess();
	}

	bDead = true;
	OnRep_Dead(); // for listen server

	SetLifeSpan(10.0f);
}


void ATPSCharacter::SetAimingDownsight_Implementation(bool bIsAiminhDownsight)
{
	bAimingDownsight = bIsAiminhDownsight;
}


void ATPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetLocalRole() == ROLE_Authority)
	{
		WeaponComp->UpdateWeaponAimDirection(FollowCamera->GetComponentLocation());
	}
}

