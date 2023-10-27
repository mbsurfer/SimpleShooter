// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Engine/DamageEvents.h"

// Sets default values
AGun::AGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Base"));
	SetRootComponent(BaseComponent);

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(BaseComponent);
}

void AGun::PullTrigger()
{
	// Add some effects
	UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, MeshComponent, TEXT("MuzzleFlashSocket"));
	UGameplayStatics::SpawnSoundAttached(MuzzleSound, MeshComponent, TEXT("MuzzleFlashSocket"));

	FHitResult HitResult;
	FVector ShotDirection;

	if (GunTrace(HitResult, ShotDirection))
	{
		// Add some effects
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactFlash, HitResult.Location, ShotDirection.Rotation());
		UGameplayStatics::SpawnSoundAtLocation(this, ImpactSound, HitResult.Location);

		// Do some damage if player hit something
		AActor* HitActor = HitResult.GetActor();
		if (HitActor)
		{
			// Create a damage event
			FPointDamageEvent DamageEvent(Damage, HitResult, ShotDirection, nullptr);

			// Do the damage
			HitActor->TakeDamage(Damage, DamageEvent, GetOwnerController(), this);
		}
	}
	
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AGun::GunTrace(FHitResult& HitResult, FVector& ShotDirection)
{
	AController* OwnerController = GetOwnerController();
	if (!OwnerController)
	{
		return false;
	}

	FVector ViewPointLocation;
	FRotator ViewPointRotation;
	OwnerController->GetPlayerViewPoint(ViewPointLocation, ViewPointRotation);
	ShotDirection = -ViewPointRotation.Vector();

	// Calculate the end point of the bullet
	FVector End = ViewPointLocation + ViewPointRotation.Vector() * MaxRange;

	// Record what the shooter hit, but make sure it can't hit itself
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredActor(GetOwner());

	return GetWorld()->LineTraceSingleByChannel(HitResult, ViewPointLocation, End, ECollisionChannel::ECC_GameTraceChannel1, CollisionParams);
}

AController* AGun::GetOwnerController() const
{
	// Cast to APawn so that we can get its controller
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) 
	{
		return nullptr;
	}

	return OwnerPawn->GetController();
}

