// Fill out your copyright notice in the Description page of Project Settings.

#include "Control/GyroMovementControlBase.h"

// Sets default values
AGyroMovementControlBase::AGyroMovementControlBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

// Called when the game starts or when spawned
void AGyroMovementControlBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGyroMovementControlBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsValid(motionControllerComp) && IsValid(controlledActor))
	{
		auto mcRorator = motionControllerComp->GetComponentRotation();
		curMCForward = mcRorator.Vector();
		curMCForwardProjection = FVector::VectorPlaneProject(curMCForward, curPlaneNormal);

		float curMCForwardProjectionLength = curMCForwardProjection.Size();
		if (curMCForwardProjectionLength >= moveOriginThreshold)
		{
			auto moveDir = FVector::VectorPlaneProject(curMCForwardProjection, FVector::UpVector);
			moveDir.Normalize();
			originalControlledActorLocation += (moveDir * curMCForwardProjectionLength*moveOriginSpeed*DeltaTime);
		}

		float forwardAmount = -1.0 * FVector::DotProduct(curPlaneLocalY, curMCForwardProjection) * moveRadius;
		float sideAmount = FVector::DotProduct(curPlaneLocalX, curMCForwardProjection) * moveRadius;

		controlledActor->SetActorLocation(originalControlledActorLocation + FVector(forwardAmount, sideAmount, 0.0f));
	}
	else
	{
		SetActorTickEnabled(false);
	}
}

void AGyroMovementControlBase::SetupCurrentMotionControllerState()
{
	if (IsValid(motionControllerComp) && IsValid(controlledActor))
	{
		auto mcRorator = motionControllerComp->GetComponentRotation();
		curPlaneNormal = mcRorator.Vector();
		curPlaneLocalX = FRotationMatrix(mcRorator).GetScaledAxis(EAxis::Y); 
		curPlaneLocalY = FRotationMatrix(mcRorator).GetScaledAxis(EAxis::Z);

		originalControlledActorLocation = controlledActor->GetActorLocation();
	}
}

