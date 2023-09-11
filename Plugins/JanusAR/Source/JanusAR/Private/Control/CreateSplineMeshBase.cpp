// Fill out your copyright notice in the Description page of Project Settings.

#include "Control/CreateSplineMeshBase.h"

// Sets default values
ACreateSplineMeshBase::ACreateSplineMeshBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

// Called when the game starts or when spawned
//void ACreateSplineMeshBase::BeginPlay()
//{
//	Super::BeginPlay();
//	
//}

// Called every frame
//void ACreateSplineMeshBase::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

void ACreateSplineMeshBase::AddSplineMeshComps(FString name, const TArray<USplineMeshComponent*>& comps)
{
	if (visualSplineMesh.Contains(name))
	{
		visualSplineMesh[name].Empty();
	}
	else
	{
		visualSplineMesh.Add(name, TArray<USplineMeshComponent*>());
	}
	visualSplineMesh[name] = comps;
}

TArray<USplineMeshComponent*> ACreateSplineMeshBase::GetSplineMeshComps(FString name)
{
	if (visualSplineMesh.Contains(name))
	{
		return visualSplineMesh[name];
	}
	return TArray<USplineMeshComponent*>();
}

void ACreateSplineMeshBase::ClearSplineMeshComps(FString name)
{
	if (visualSplineMesh.Contains(name))
	{
		for (auto comp : visualSplineMesh[name])
		{
			comp->DestroyComponent();
		}
		visualSplineMesh[name].Empty();
	}
}
