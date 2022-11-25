// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/SPickupActor.h"

#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "Powerups/SPowerupActor.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

// Sets default values
ASPickupActor::ASPickupActor()
{
 	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(75.0f);
	RootComponent = SphereComp;
	
	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	DecalComp->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	DecalComp->DecalSize = FVector(64, 75, 75);
	DecalComp->SetupAttachment(RootComponent);

	CooldownDuration = 10.0f;

	bReplicates = true;
}

// Called when the game starts or when spawned
void ASPickupActor::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority())
	{
		Respawn();
	}
}

void ASPickupActor::Respawn()
{
	if(PowerupClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PowerupClass is nullptr in %s. Please update your Blueprint"), *GetName());
		return;
	}
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	PowerupInstance = GetWorld()->SpawnActor<ASPowerupActor>(PowerupClass, GetTransform(), SpawnParams);
}

void ASPickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if(HasAuthority() && PowerupInstance)
	{
		PowerupInstance->ActivatePowerup(OtherActor);
		PowerupInstance = nullptr;

		// set timer to respawn
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &ASPickupActor::Respawn, CooldownDuration);
	}
}
