// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/ELClientActor.h"

// Sets default values
AELClientActor::AELClientActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}


void AELClientActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (GetWorld()->GetNetMode() == NM_Client)
	{
		DispatchBeginPlay();
	}
}

