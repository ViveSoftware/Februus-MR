// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/ELWorldBase.h"
#include "Framework/ELSystemBase.h"
#include "Net/UnrealNetwork.h"

//DEFINE_LOG_CATEGORY_STATIC(LogELGameFramework, Log, All)

// Sets default values
AELWorldBase::AELWorldBase()
{
	bReplicates = true;
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AELWorldBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AELWorldBase, name);
	DOREPLIFETIME(AELWorldBase, actors);
	DOREPLIFETIME(AELWorldBase, systems);
	DOREPLIFETIME(AELWorldBase, systemsMapConstructed);
	DOREPLIFETIME(AELWorldBase, enable);
	//DOREPLIFETIME(AELWorldBase, systemsMap);
}

bool AELWorldBase::SetEnable_Validate(bool value)
{
	return enable != value;
}

void AELWorldBase::SetEnable_Implementation(bool value)
{
	enable = value;
	for (auto system : systems)
	{
		system->SetEnable(enable);
	}
}

bool AELWorldBase::AddActor_Validate(AActor * actor)
{
	return actor != nullptr;
}

void AELWorldBase::AddActor_Implementation(AActor * actor)
{
	checkf(actor != nullptr, TEXT("[%s][AddActor] actor is null"), *this->GetName());

	UE_LOG(LogELGameFramework, Log, TEXT("[%s] %s join world."), *this->GetName(), *actor->GetName());

	for (AELSystemBase* system : systems)
	{
		system->AddActor(actor);
	}

	if (actors.Contains(actor) == false)
	{
		actors.Add(actor);
		actor->OnDestroyed.AddDynamic(this, &AELWorldBase::OnActorDestroyed);
	}
}

bool AELWorldBase::RemoveActor_Validate(AActor * actor)
{
	return actor != nullptr;
}

void AELWorldBase::RemoveActor_Implementation(AActor* actor)
{
	checkf(actor != nullptr, TEXT("[%s][RemoveActor] actor is null"), *this->GetName());
	if (actors.Contains(actor))
	{
		actors.Remove(actor);
		actor->OnDestroyed.RemoveDynamic(this, &AELWorldBase::OnActorDestroyed);
	}
	for (auto system : systems)
	{
		system->RemoveActor(actor);
	}

	UE_LOG(LogELGameFramework, Log, TEXT("[%s] %s leave world."), *this->GetName(), *actor->GetName());
}

bool AELWorldBase::AddSystem_Validate(AELSystemBase * system)
{
	return system != nullptr;
}

void AELWorldBase::AddSystem_Implementation(AELSystemBase * system)
{
	checkf(system != nullptr, TEXT("[%s][AddSystem] system is null"), *this->GetName());
	FString sysName = system->GetSystemName();
	if (systemsMap.Contains(sysName))
	{
		UE_LOG(LogELGameFramework, Error, TEXT("[%s] ELSystem-%s is already in the world."), *this->GetName(), *system->GetName());
		redundantSystems.Add(system);
	}
	else
	{
		systems.Add(system);
		systemsMap.Add(sysName, system);
	}

	for (auto actor : actors)
	{
		system->AddActor(actor);
	}
}

bool AELWorldBase::PostAddComponent_Validate(AActor* actor)
{
	return actor != nullptr;
}

void AELWorldBase::PostAddComponent_Implementation(AActor* actor)
{
	checkf(actor != nullptr, TEXT("[%s][PostAddComponent] actor is null"), *this->GetName());
	for (AELSystemBase* system : systems)
	{
		system->AddActor(actor);
	}
}

bool AELWorldBase::PostRemoveComponent_Validate(AActor* actor)
{
	return actor != nullptr;
}

void AELWorldBase::PostRemoveComponent_Implementation(AActor* actor)
{
	checkf(actor != nullptr, TEXT("[%s][PostRemoveComponent] actor is null"), *this->GetName());
	for (auto system : systems)
	{
		system->CheckRemoveActor(actor);
	}
}

bool AELWorldBase::ActorIsInWorld(AActor* actor)
{
	checkf(actor != nullptr, TEXT("[%s][ActorIsInWorld] actor is null"), *this->GetName());

	return actors.Contains(actor);
}

AELSystemBase* AELWorldBase::GetSystem(FString systemName)
{
	if (systemsMap.Num() == 0) ConstructSystemsMap();
	if (systemsMap.Contains(systemName))
	{
		return systemsMap[systemName];
	}
	return nullptr;
}

// Called when the game starts or when spawned
void AELWorldBase::BeginPlay()
{
	if (HasAuthority())
	{
		if (!systemsMapConstructed) ConstructSystemsMap();

		TArray<AActor*> tmpActors(actors);
		for (AActor* actor : tmpActors)
		{
			AddActor(actor);
		}
	}

	Super::BeginPlay();

#if WITH_EDITOR
	PrintWorldAndSystemInfo();
#endif
}

void AELWorldBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (name.IsEmpty())
	{
		UE_LOG(LogELGameFramework, Warning, TEXT("[%s] ELWorld's name is empty."), *this->GetName());
	}

	if (HasAuthority())
	{
		systemsMapConstructed = false;
		ConstructSystemsMap();
	}

#if WITH_EDITOR
	PrintWorldAndSystemInfo();
#endif
}

void AELWorldBase::ConstructSystemsMap()
{
	// clear map and system info
	for (auto sysItr : systemsMap)
	{
		if (sysItr.Value != nullptr)
		{
			sysItr.Value->myWorld = nullptr;
		}
	}
	systemsMap.Empty();
	redundantSystems.Empty();

	for (AELSystemBase* system : systems)
	{
		if (system == nullptr) continue;

		if (system->myWorld != nullptr)
		{
			UE_LOG(LogELGameFramework, Error, TEXT("[%s] ELSystem-%s is in world-%s, and is ignored by the world-%s."), *this->GetName(), *system->GetName(), *system->myWorld->GetName(), *this->GetName());
			redundantSystems.Add(system);
			continue;
		}

		FString sysName = system->GetSystemName();
		if (sysName.IsEmpty())
		{
			UE_LOG(LogELGameFramework, Error, TEXT("[%s] ELSystem-%s's name is empty, and is ignored by the world."), *this->GetName(), *system->GetName());
			continue;
		}
		if (systemsMap.Contains(sysName))
		{
			UE_LOG(LogELGameFramework, Error, TEXT("[%s] ELSystem-%s's name-\"%s\" is already exist in this world, and is ignored by the world."), *this->GetName(), *system->GetName(), *sysName);
			redundantSystems.Add(system);
		}
		else
		{
			system->myWorld = this;
			systemsMap.Add(sysName, system);
		}
	}

	for (AELSystemBase* system : redundantSystems)
	{
		systems.Remove(system);
	}

	systemsMapConstructed = true;
}

void AELWorldBase::OnActorDestroyed(AActor * actor)
{
	RemoveActor(actor);
}

#if WITH_EDITOR
void AELWorldBase::PrintWorldAndSystemInfo()
{
	UE_LOG(LogELGameFramework, Log, TEXT("[%s] %s has systems:"), *this->GetName(), *name);

	for (AELSystemBase* system : systems)
	{
		if (system == nullptr) continue;
		UE_LOG(LogELGameFramework, Log, TEXT("\t[%s] %s"), *system->GetName(), *system->GetSystemName());
	}
}
#endif

