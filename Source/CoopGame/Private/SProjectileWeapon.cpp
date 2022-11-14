// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectileWeapon.h"

void ASProjectileWeapon::Fire()
{
	// trace the world from pawn eyes to crosshair location
	AActor* MyOwner = GetOwner();
	if(MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
	}
}
