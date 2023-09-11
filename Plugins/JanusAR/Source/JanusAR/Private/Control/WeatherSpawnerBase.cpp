// Fill out your copyright notice in the Description page of Project Settings.

#include "Control/WeatherSpawnerBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AWeatherSpawnerBase::AWeatherSpawnerBase():
	FingerSpeedThreshold(0.25f),
	HideFingerTrailWhenFarMode(false),
	EnableManualSpawn(false),
	RandomDelayMin(0.1f),
	RandomDelayMax(0.2f),
	LocationRandomRange(5.0f),
	WeatherParticleWeight(0.2f),
	DistanceThreshold(5.0f)
{
	bReplicates = true;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));

	Range = CreateDefaultSubobject<UBoxComponent>(TEXT("Range"));
	Range->SetupAttachment(RootComponent);

	Effector = CreateDefaultSubobject<USphereComponent>(TEXT("Effector"));
	Effector->SetupAttachment(RootComponent);

	FingerTrail = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FingerTrail"));
	FingerTrail->SetupAttachment(RootComponent);

	TrailDecoration = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TrailDecoration"));
	TrailDecoration->SetupAttachment(FingerTrail);

	FarTrail = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FarTrail"));
	FarTrail->SetupAttachment(RootComponent);
}

void AWeatherSpawnerBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeatherSpawnerBase, CurrentPos);
	DOREPLIFETIME(AWeatherSpawnerBase, LastPos);
	DOREPLIFETIME(AWeatherSpawnerBase, MiddlePos);
	//DOREPLIFETIME(AWeatherSpawnerBase, EnableParticleEffect);
	DOREPLIFETIME(AWeatherSpawnerBase, CurrentSpeed);
	//DOREPLIFETIME(AWeatherSpawnerBase, EnableManualSpawn);
	//DOREPLIFETIME(AWeatherSpawnerBase, PlaneHeight);
	//DOREPLIFETIME(AWeatherSpawnerBase, particleSpawnTransform);
	DOREPLIFETIME(AWeatherSpawnerBase, EffectorPos);
	DOREPLIFETIME(AWeatherSpawnerBase, IsUpdating);
}

// Called when the game starts or when spawned
void AWeatherSpawnerBase::BeginPlay()
{
	Super::BeginPlay();
	FingerTrail->EndTrails();
	FarTrail->EndTrails();
	TrailDecoration->EndTrails();
}

// Called every frame
void AWeatherSpawnerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (tickGateBool)
	{
		if (GetWorld()->IsNetMode(ENetMode::NM_Standalone) || GetWorld()->IsNetMode(ENetMode::NM_ListenServer))
		{
			if (startUpdateDoOnce) OnStartUpdate();
			UpdateCurFingerPos(false);
			UpdateTrail();
			UpdateParticle();
		}
	}
}

void AWeatherSpawnerBase::OpenTickGate()
{
	tickGateBool = true;
}

void AWeatherSpawnerBase::CloseTickGate()
{
	tickGateBool = false;
}

UParticleSystem* AWeatherSpawnerBase::GetSpawnParticle_Implementation()
{
	int index = 0;
	if (SpawnParticles.Num() > 1 && !UKismetMathLibrary::RandomBoolWithWeight(0.7f))
	{
		index = UKismetMathLibrary::RandomIntegerInRange(1, SpawnParticles.Num() - 1);
	}

	return SpawnParticles[index];
}

FVector AWeatherSpawnerBase::GetSpawnLocation_Implementation()
{
	FVector v = UKismetMathLibrary::RandomUnitVector() * UKismetMathLibrary::RandomFloatInRange(0.0f, LocationRandomRange);
	FVector location = CurrentPos + v;;
	if (FarMode)
	{
		location = GetFarModePos(location);
	}

	return location;
}

FRotator AWeatherSpawnerBase::GetSpawnRotation_Implementation()
{
	return FRotator::ZeroRotator;
}

FVector AWeatherSpawnerBase::GetFarModePos_Implementation(FVector Pos)
{
	return FVector::ZeroVector;
}

float AWeatherSpawnerBase::GetCurrentFingerSpeed_Implementation()
{
	float speed = UKismetMathLibrary::MapRangeClamped(FVector::Dist(CurrentPos, LastPos), 0.0f, 15.0f, 0.0f, 1.0f);
	return speed;
}

void AWeatherSpawnerBase::UpdateTrail_Implementation()
{
	FingerTrail->SetWorldLocation(CurrentPos, false, nullptr, ETeleportType::TeleportPhysics);

	UpdateTrailMulticast();

	// FarMode not handle network
	if (FarMode)
	{
		FarTrail->SetWorldLocation(GetFarModePos(CurrentPos), false, nullptr, ETeleportType::TeleportPhysics);
		if (farTrailDoOnce)
		{
			if (HideFingerTrailWhenFarMode) FingerTrail->EndTrails();
			FarTrail->BeginTrails("None", "None", ETrailWidthMode::ETrailWidthMode_FromCentre, 0.0f);
			TrailDecoration->AttachToComponent(FarTrail, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			endFarTrailDoOnce = true;
			farTrailDoOnce = false;
		}
	}
	else
	{
		if (endFarTrailDoOnce)
		{
			if (HideFingerTrailWhenFarMode) FingerTrail->BeginTrails("None", "None", ETrailWidthMode::ETrailWidthMode_FromCentre, 0.0f);
			FarTrail->EndTrails();
			TrailDecoration->AttachToComponent(FingerTrail, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			farTrailDoOnce = true;
			endFarTrailDoOnce = false;
		}
	}
}

void AWeatherSpawnerBase::UpdateTrailMulticast_Implementation()
{
	OnUpdateTrailMulticast();
}

void AWeatherSpawnerBase::UpdateParticle_Implementation()
{
	CurrentSpeed = GetCurrentFingerSpeed();
	if (CurrentSpeed >= FingerSpeedThreshold && !MiddlePos.Equals(CurrentPos, DistanceThreshold / 1.5f))
	{
		UpdateParticleSpawn();
	}
}

void AWeatherSpawnerBase::UpdateParticleSpawn_Implementation()
{
	SpawnWeatherParticle();
}

void AWeatherSpawnerBase::SpawnWeatherParticle_Implementation()
{
	if (UKismetMathLibrary::RandomBoolWithWeight(WeatherParticleWeight))
	{
		particleSpawnTransform = FTransform(GetSpawnRotation(), GetSpawnLocation());
		SpawnWeatherParticleMulticast();

		// old single mode
		//FVector location = GetSpawnLocation();
		//if (EnableManualSpawn)
		//{
		//	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GetSpawnParticle(), FTransform(GetSpawnRotation(), location));
		//}
		//if (EnableParticleEffect)
		//{
		//	Effector->SetWorldLocation(FVector(location.X, location.Y, PlaneHeight));
		//}
	}
}

void AWeatherSpawnerBase::SpawnWeatherParticleMulticast_Implementation()
{
	if (EnableManualSpawn)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GetSpawnParticle(), particleSpawnTransform);
	}
	if (EnableParticleEffect)
	{
		EffectorPos = FVector(particleSpawnTransform.GetLocation().X, particleSpawnTransform.GetLocation().Y, PlaneHeight);
		Effector->SetWorldLocation(EffectorPos);
	}
}

void AWeatherSpawnerBase::OnStartUpdate_Implementation()
{
	IsUpdating = true;
	UpdateCurFingerPos(true);
	FingerTrail->BeginTrails("None", "None", ETrailWidthMode::ETrailWidthMode_FromCentre, 0.0f);
	TrailDecoration->BeginTrails("None", "None", ETrailWidthMode::ETrailWidthMode_FromCentre, 0.0f);
	
	if (EnableParticleEffect)
	{
		Effector->SetWorldLocation(FVector(Effector->GetComponentLocation().X, Effector->GetComponentLocation().Y, -500.0f));
		Effector->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	OnStartUpdateMulticast();

	startUpdateDoOnce = false;
}

void AWeatherSpawnerBase::OnStartUpdateMulticast_Implementation()
{
	OnStartUpdateTrailMulticast();
}

void AWeatherSpawnerBase::OnEndUpdate_Implementation()
{
	IsUpdating = false;
	startUpdateDoOnce = true;
	FingerTrail->EndTrails();
	TrailDecoration->EndTrails();
	
	if (FarMode)
	{
		FarTrail->EndTrails();
		farTrailDoOnce = true;
		TrailDecoration->AttachToComponent(FingerTrail, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}

	if (EnableParticleEffect)
	{
		Effector->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Effector->SetWorldLocation(FVector(Effector->GetComponentLocation().X, Effector->GetComponentLocation().Y, -500.0f));
	}

	OnEndUpdateMulticast();
}

void AWeatherSpawnerBase::OnEndUpdateMulticast_Implementation()
{
	OnEndUpdateTrailMulticast();
}

void AWeatherSpawnerBase::UpdateCurFingerPos_Implementation(bool UpdateLast)
{
}
