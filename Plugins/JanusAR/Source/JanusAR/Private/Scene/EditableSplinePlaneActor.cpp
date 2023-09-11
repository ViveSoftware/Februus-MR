// Fill out your copyright notice in the Description page of Project Settings.

#include "Scene/EditableSplinePlaneActor.h"

// Sets default values
AEditableSplinePlaneActor::AEditableSplinePlaneActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

//// Called when the game starts or when spawned
//void AEditableSplinePlaneActor::BeginPlay()
//{
//	Super::BeginPlay();
//	
//}
//
//// Called every frame
//void AEditableSplinePlaneActor::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}



void AEditableSplinePlaneActor::UpdateSplineLocation_Implementation(int point_index, FVector location, ESplineCoordinateSpace::Type coordinate_space)
{
	if (IsValid(spline))
	{
		spline->SetLocationAtSplinePoint(point_index, location, coordinate_space);
	}
}
