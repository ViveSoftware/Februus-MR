// Fill out your copyright notice in the Description page of Project Settings.

#include "Control/EditableSplinePlaneModifier.h"

// Sets default values
AEditableSplinePlaneModifier::AEditableSplinePlaneModifier()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	originalSpline = CreateDefaultSubobject<USplineComponent>(TEXT("OriginalSplineInfo"));
	RootComponent = originalSpline;
}

// Called when the game starts or when spawned
//void AEditableSplinePlaneModifier::BeginPlay()
//{
//	Super::BeginPlay();
//	
//}

// Called every frame
//void AEditableSplinePlaneModifier::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

void AEditableSplinePlaneModifier::SetupOriginalSpline_Implementation()
{
	if (IsValid(targetSpline))
	{
		originalSpline->ClearSplinePoints();
		splinePointOriginalLocation.Empty();
		int splineNum = targetSpline->GetNumberOfSplinePoints();
		for (int i = 0; i < splineNum; i++)
		{
			auto location = targetSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
			originalSpline->AddSplinePoint(location, ESplineCoordinateSpace::World, false);
			splinePointOriginalLocation.Add(location);
		}
		originalSpline->SetClosedLoop(targetSpline->IsClosedLoop());
	}
}

void AEditableSplinePlaneModifier::SetupCenterSplinePointInfo_Implementation()
{
	auto startLocationOnSpline = originalSpline->FindLocationClosestToWorldLocation(startLocation, ESplineCoordinateSpace::World);
	auto startKeyOnSpline = originalSpline->FindInputKeyClosestToWorldLocation(startLocationOnSpline);
	float startKeyOnSpline_ciel_dist = originalSpline->GetDistanceAlongSplineAtSplinePoint(FMath::CeilToInt(startKeyOnSpline));
	float startKeyOnSpline_floor_dist = originalSpline->GetDistanceAlongSplineAtSplinePoint(FMath::CeilToInt(startKeyOnSpline));
	centerDistance = startKeyOnSpline_floor_dist + FMath::Frac(startKeyOnSpline) * (startKeyOnSpline_ciel_dist - startKeyOnSpline_floor_dist);
	if (specifiedDirection)
	{
		modifiedDirection = originalSpline->FindRightVectorClosestToWorldLocation(startLocationOnSpline, ESplineCoordinateSpace::World);
	}
}

void AEditableSplinePlaneModifier::CalculateAllSplinePointDistance_Implementation()
{
	int splineNum = originalSpline->GetNumberOfSplinePoints();
	float splineLength = originalSpline->GetSplineLength();
	bool isClosedLoop = originalSpline->IsClosedLoop();
	splinePointDistanceToModifyingCenter.Init(0.0f, splineNum);
	for (int i = 0; i < splineNum; i++)
	{
		float distance = originalSpline->GetDistanceAlongSplineAtSplinePoint(i);
		splinePointDistanceToModifyingCenter[i] = FMath::Abs(distance - centerDistance);
		if (isClosedLoop)
		{
			float dist1 = FMath::Abs(splineLength + distance - centerDistance);
			float dist2 = FMath::Abs(splineLength + centerDistance - distance);
			splinePointDistanceToModifyingCenter[i] = FMath::Min3(splinePointDistanceToModifyingCenter[i], dist1, dist2);
		}
	}
}

void AEditableSplinePlaneModifier::UpdateModifyingSplinePoints_Implementation()
{
	modifyingSplinePoints.Empty();
	int splineNum = targetSpline->GetNumberOfSplinePoints();
	for (int i = 0; i < splineNum; i++)
	{
		if (splinePointDistanceToModifyingCenter[i] <= currentRange)
		{
			modifyingSplinePoints.Add(i);
		}
	}
}

void AEditableSplinePlaneModifier::UpdateModifiying_Implementation(FVector location)
{
	currentLocation = location;
	FVector moveVector = currentLocation - startLocation;
	if (specifiedDirection)
	{
		moveVector = modifiedDirection * FVector::DotProduct(moveVector, modifiedDirection);
	}

	for (auto pointIndex : modifyingSplinePoints)
	{
		float modifiedAmount = splinePointDistanceToModifyingCenter[pointIndex] / currentRange;
		if (IsValid(locationRatioCurve))
		{
			modifiedAmount = locationRatioCurve->GetFloatValue(modifiedAmount);
		}
		FVector modifiedSplinePointLocation = splinePointOriginalLocation[pointIndex] + FVector(moveVector.X * modifiedAmount, moveVector.Y * modifiedAmount, 0.0f);
		targetSpline->SetLocationAtSplinePoint(pointIndex, modifiedSplinePointLocation, ESplineCoordinateSpace::World, false);
	}
	targetSpline->UpdateSpline();

	currentRange = rangeFactor * moveVector.Size() + originalRange;
	UpdateModifyingSplinePoints();

	if (updateStartLocation)
	{
		startLocation = originalSpline->FindLocationClosestToWorldLocation(currentLocation, ESplineCoordinateSpace::World);
		SetupCenterSplinePointInfo();
		CalculateAllSplinePointDistance();
	}
}

void AEditableSplinePlaneModifier::StartModifiying_Implementation(USplineComponent * spline_, FVector startLocation_, AEditableSplinePlaneActor * editable_spline_plane_actor)
{
	startLocation = startLocation_;
	targetSpline = spline_;
	targetEditableSplinePlaneActor = editable_spline_plane_actor;

	SetupOriginalSpline();
	SetupCenterSplinePointInfo();
	CalculateAllSplinePointDistance();
	UpdateModifyingSplinePoints();
}

void AEditableSplinePlaneModifier::EndModifiying_Implementation()
{
}
