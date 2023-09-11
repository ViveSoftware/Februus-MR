// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/ELFrameworkHelperComponent.h"
#include "Engine.h"

// Sets default values for this component's properties
UELFrameworkHelperComponent::UELFrameworkHelperComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	// ...
}


AELWorldBase * UELFrameworkHelperComponent::GetGameWorld(FString name)
{
	if (GameWorldsMap.Num() == 0)
	{
		ConstructWorldMap();
	}
	if (GameWorldsMap.Contains(name))
	{
		return GameWorldsMap[name];
	}
	else
	{
		return nullptr;
	}
}

// Called when the game starts
void UELFrameworkHelperComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GameWorldsMap.Num() == 0)
	{
		ConstructWorldMap();
	}

	// ...

}

void UELFrameworkHelperComponent::PostInitProperties()
{
	Super::PostInitProperties();

	ConstructWorldMap();
}

void UELFrameworkHelperComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GameWorlds.Empty();
	GameWorldsMap.Empty();
}

void UELFrameworkHelperComponent::ConstructWorldMap()
{
	GameWorlds.Empty();
	GameWorldsMap.Empty();

	TArray<AActor*> foundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AELWorldBase::StaticClass(), foundActors);

	for (AActor* actor : foundActors)
	{
		AELWorldBase* elworld = Cast<AELWorldBase>(actor);
		GameWorlds.Add(elworld);
		GameWorldsMap.Add(elworld->GetWorldName(), elworld);
	}
}


