// Fill out your copyright notice in the Description page of Project Settings.

#include "Control/BulletShieldBase.h"

// Sets default values
ABulletShieldBase::ABulletShieldBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABulletShieldBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABulletShieldBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

