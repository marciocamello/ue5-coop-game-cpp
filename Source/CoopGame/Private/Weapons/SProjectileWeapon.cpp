// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/SProjectileWeapon.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

void ASProjectileWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	TimeBetweenShots = 60 / RateOfFire;
}

void ASProjectileWeapon::Fire()
{
	// trace the world from pawn eyes to crosshair location
	AActor* MyOwner = GetOwner();
	if(MyOwner && ProjectileClass)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if(FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, GetActorLocation());
		}
		
		GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, EyeRotation, SpawnParams);
		
		LastFireTime = GetWorld()->TimeSeconds;
	}
}

void ASProjectileWeapon::StartFire()
{
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
	
	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASProjectileWeapon::Fire, TimeBetweenShots,true, FirstDelay);
}

void ASProjectileWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}