// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/SExplosiveBarrel.h"

#include "Components/SHealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASExplosiveBarrel::OnHealthChanged);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetSimulatePhysics(true);
	// set to physics body to let radial component affetc us (eg. when a nearby barrel explodes)
	MeshComp->SetCollisionObjectType(ECC_PhysicsBody);
	RootComponent = MeshComp;

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	RadialForceComp->SetupAttachment(MeshComp);
	RadialForceComp->Radius = 250;
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->bAutoActivate = false; // prevent component from ticking, and only use FireImpulse() instead
	RadialForceComp->bIgnoreOwningActor = true; // ignore self

	ExplosionImpulse = 400;
	ExplosionDamage = 40;
	
	SetReplicates(true);
	SetReplicateMovement(true);
	
	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;
}

void ASExplosiveBarrel::OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta,
	const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if(bExploded)
	{
		// nothing left to do, already exploded
		return;
	}

	if(Health <= 0.0f)
	{
		// Explode!
		bExploded = true;
		OnRep_Exploded();

		// Boost the barrel upwards
		FVector BoostIntensity = FVector::UpVector * ExplosionImpulse;
		MeshComp->AddImpulse(BoostIntensity, NAME_None, true);

		// Blast away nearby physics actors
		RadialForceComp->FireImpulse();
	}

	// Apply Damage
	UGameplayStatics::ApplyRadialDamage(GetWorld(), ExplosionDamage, GetActorLocation(), RadialForceComp->Radius,
		UDamageType::StaticClass(), TArray<AActor*>(), this, GetInstigatorController(), true);
	
}

void ASExplosiveBarrel::OnRep_Exploded()
{
	// play FX and change self material to black
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	// override material on mesh with blackened version
	MeshComp->SetMaterial(0, ExplodedMaterial);
	// play sound
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());
}

void ASExplosiveBarrel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASExplosiveBarrel, bExploded);
}