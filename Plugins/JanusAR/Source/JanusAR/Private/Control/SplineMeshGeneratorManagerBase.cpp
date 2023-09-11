// Fill out your copyright notice in the Description page of Project Settings.

#include "Control/SplineMeshGeneratorManagerBase.h"

// Sets default values
ASplineMeshGeneratorManagerBase::ASplineMeshGeneratorManagerBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

// Called when the game starts or when spawned
void ASplineMeshGeneratorManagerBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASplineMeshGeneratorManagerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

