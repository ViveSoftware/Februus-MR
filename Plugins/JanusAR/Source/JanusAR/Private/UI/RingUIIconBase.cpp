// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/RingUIIconBase.h"

// Sets default values
ARingUIIconBase::ARingUIIconBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
}


