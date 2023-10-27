// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/CapsuleComponent.h"
#include "SimpleShooterGameModeBase.h"
#include "Gun.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AShooterCharacter::Shoot()
{
	// UE_LOG(LogTemp, Warning, TEXT("Shots fired!"));
	Gun->PullTrigger();
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Setup enhanced inputs
	APlayerController* ShooterController = Cast<APlayerController>(GetController());
	if (ShooterController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ShooterController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(ShooterMappingContext, 0);
        }
	}

	// Initialize player health
	Health = MaxHealth;

	// Spawn a rifle into the world. The GunClass is assigned in using the Blueprint class BP_ShooterCharacter.
	Gun = GetWorld()->SpawnActor<AGun>(GunClass);

	// Hide the default gun by name
	GetMesh()->HideBoneByName(TEXT("weapon_r"), EPhysBodyOp::PBO_None);

	// Attach the rifle to the shooter using the weapon socket that was created on the Wraith_Skeleton
	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));

	// Assign owner for damage source
	Gun->SetOwner(this);
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputAction = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputAction->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &AShooterCharacter::MoveForward);
		EnhancedInputAction->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &AShooterCharacter::MoveRight);
		EnhancedInputAction->BindAction(LookAction, ETriggerEvent::Triggered, this, &AShooterCharacter::Look);
		EnhancedInputAction->BindAction(JumpAction, ETriggerEvent::Started, this, &AShooterCharacter::JumpInput);
		EnhancedInputAction->BindAction(ShootAction, ETriggerEvent::Started, this, &AShooterCharacter::ShootInput);
    }

}

float AShooterCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float DamageTaken = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// Player health should not drop below 0
	DamageTaken = FMath::Min(Health, DamageTaken);
	Health -= DamageTaken;
	
	UE_LOG(LogTemp, Warning, TEXT("TakeDamage triggered. Current Health: %f"), Health);

	// If shooter is dead...
	if (IsDead())
	{
		// Inform the game mode which pawn was killed
		ASimpleShooterGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ASimpleShooterGameModeBase>();
		if (GameMode)
		{
			// IMPORTANT: This must be called before detatching the controller
			GameMode->PawnKilled(this);
		}

		// Detatch the controller
		DetachFromControllerPendingDestroy();

		// Remove collision
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	return DamageTaken;
}

bool AShooterCharacter::IsDead() const
{
	return Health <= 0;
}

float AShooterCharacter::GetHealthPercent() const
{
	return Health / MaxHealth;
}

void AShooterCharacter::MoveForward(const FInputActionValue& Value)
{
	const float CurrentValue = Value.Get<float>();
	AddMovementInput(GetActorForwardVector() * CurrentValue);
}

void AShooterCharacter::MoveRight(const FInputActionValue& Value)
{
	const float CurrentValue = Value.Get<float>();
	AddMovementInput(GetActorRightVector() * CurrentValue);
}

void AShooterCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D CurrentValue = Value.Get<FVector2D>();
	AddControllerYawInput(CurrentValue.X);
	AddControllerPitchInput(CurrentValue.Y);
}

void AShooterCharacter::JumpInput(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{
		Super::Jump();
	}
}

void AShooterCharacter::ShootInput(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{
		Shoot();
	}
}