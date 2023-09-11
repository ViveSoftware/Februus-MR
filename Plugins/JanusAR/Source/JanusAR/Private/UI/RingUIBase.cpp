// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/RingUIBase.h"

// Sets default values
ARingUIBase::ARingUIBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
}

// Called when the game starts or when spawned
void ARingUIBase::BeginPlay()
{
	Super::BeginPlay();

	inverseFactorY = inverseY ? -1.0f : 1.0f;
	inverseFactorZ = inverseZ ? -1.0f : 1.0f;
}

void ARingUIBase::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);

	inverseFactorY = inverseY ? -1.0f : 1.0f;
	inverseFactorZ = inverseZ ? -1.0f : 1.0f;

	int iconNum = iconActors.Num();
	if (iconNum != 0)
	{
		initDegreeOffset = 360.0f / iconNum;
	}

	iconToOriginalDegree.Empty();
	
	for (int i = 0; i < iconActors.Num(); i++)
	{
		auto iconActor = iconActors[i];
		if (IsValid(iconActor))
		{
			iconActor->AttachToActor(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
			float curDegreeOffset = i * initDegreeOffset + startDegreeOffset;
			curDegreeOffset = (curDegreeOffset > 180.0f) ? (curDegreeOffset - 360.0f) : curDegreeOffset;
			float curDegreeOffsetInRadian = (curDegreeOffset * PI) / 180.0f;
			float relativeY = radius * FMath::Sin(curDegreeOffsetInRadian) * inverseFactorY;
			float relativeZ = radius * FMath::Cos(curDegreeOffsetInRadian) * inverseFactorZ;
			iconActor->SetActorRelativeLocation(FVector(0.0f, -1.0f*relativeY, -1.0f*relativeZ));
			iconToOriginalDegree.Add(iconActor, curDegreeOffset);
		}
	}
}

// Called every frame
void ARingUIBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (scalingEffectEnable == true)
	{
		UpdateScalingEffect(DeltaTime);
	}
	else
	{
		ResetScalingEffect(DeltaTime);
		SetCurrentTouchDegree(0.0f);
	}

	//if (isTouch == true)
	//{
	//	float curTouchDegree_modified = curTouchDegree + 180.0f;
	//	curTouchDegree_modified = (curTouchDegree_modified > 180.0f) ? (curTouchDegree_modified - 360.0f) : curTouchDegree_modified;

	//	if (IsValid(currentSelected))
	//	{
	//		float iconDegree = iconToOriginalDegree[currentSelected];
	//		float distDegree = FMath::Abs(curTouchDegree_modified - iconDegree);
	//		distDegree = (distDegree > 180.0f) ? (360.0f - distDegree) : distDegree;
	//		if (distDegree > selectedDegreeThreshold)
	//		{
	//			currentSelected->OnSelectedLeave();
	//			OnSelectedLeave.Broadcast(currentSelected);
	//			currentSelected = nullptr;
	//		}
	//	}

	//	for (auto iconToDegree : iconToOriginalDegree)
	//	{
	//		auto iconActor = iconToDegree.Key;
	//		//if (IsValid(iconActor) == false) continue;
	//		float iconDegree = iconToDegree.Value;
	//		float distDegree = FMath::Abs(curTouchDegree_modified - iconDegree);
	//		distDegree = (distDegree > 180.0f) ? (360.0f - distDegree) : distDegree;
	//		if (distDegree <= selectedDegreeThreshold)
	//		{
	//			if (currentSelected != iconActor)
	//			{
	//				currentSelected = iconActor;
	//				iconActor->OnSelectedEnter();
	//				OnSelectedEnter.Broadcast(iconActor);
	//			}
	//			break;
	//		}
	//	}
	//}
	//else
	//{
	//	if (IsValid(currentSelected))
	//	{
	//		currentSelected->OnSelectedLeave();
	//		OnSelectedLeave.Broadcast(currentSelected);
	//	}
	//	currentSelected = nullptr;
	//}
}

ARingUIIconBase * ARingUIBase::GetCurrentSelected()
{
	return currentSelected;
}

void ARingUIBase::SetCurrentTouchDegree(float value)
{
	curTouchDegree = value;
	if (isTouch == true)
	{
		float curTouchDegree_modified = curTouchDegree + 180.0f;
		curTouchDegree_modified = (curTouchDegree_modified > 180.0f) ? (curTouchDegree_modified - 360.0f) : curTouchDegree_modified;

		if (IsValid(currentSelected))
		{
			float iconDegree = iconToOriginalDegree[currentSelected];
			float distDegree = FMath::Abs(curTouchDegree_modified - iconDegree);
			distDegree = (distDegree > 180.0f) ? (360.0f - distDegree) : distDegree;
			if (distDegree > selectedDegreeThreshold)
			{
				currentSelected->OnSelectedLeave();
				OnSelectedLeave.Broadcast(currentSelected);
				currentSelected = nullptr;
			}
		}

		if (currentSelected == nullptr)
		{
			for (auto iconToDegree : iconToOriginalDegree)
			{
				auto iconActor = iconToDegree.Key;
				//if (IsValid(iconActor) == false) continue;
				float iconDegree = iconToDegree.Value;
				float distDegree = FMath::Abs(curTouchDegree_modified - iconDegree);
				distDegree = (distDegree > 180.0f) ? (360.0f - distDegree) : distDegree;
				if (distDegree <= selectedDegreeThreshold)
				{
					if (currentSelected != iconActor)
					{
						currentSelected = iconActor;
						iconActor->OnSelectedEnter();
						OnSelectedEnter.Broadcast(iconActor);
					}
					break;
				}
			}
		}
	}
	else
	{
		if (IsValid(currentSelected))
		{
			currentSelected->OnSelectedLeave();
			OnSelectedLeave.Broadcast(currentSelected);
		}
		currentSelected = nullptr;
	}
}

void ARingUIBase::SetScalingEffectEnable(bool value)
{
	if (scalingEffectEnable == true && value == false)
	{
		alreadyReset = false;
	}
	scalingEffectEnable = value;
}

void ARingUIBase::UpdateScalingEffect_Implementation(float DeltaTime)
{
	for (auto iconToDegree : iconToOriginalDegree)
	{
		auto iconActor = iconToDegree.Key;
		//if (IsValid(iconActor) == false) continue;
		float originalDegree = iconToDegree.Value;
		float distDegree = curTouchDegree - originalDegree;

		float distDegree_modifiedForOffset = (distDegree > 180.0f) ? (180.0f - distDegree) : distDegree;
		distDegree_modifiedForOffset = (distDegree_modifiedForOffset < -180.0f) ? (distDegree_modifiedForOffset + 360.0f) : distDegree_modifiedForOffset;
		float offsetDegree = scalingDegreeOffset * degreeOffsetCurve->GetFloatValue(FMath::Abs(distDegree_modifiedForOffset));
		float newDegree = (distDegree_modifiedForOffset < 0.0f) ? (originalDegree - offsetDegree) : (originalDegree + offsetDegree);
		float newDegreeInRadian = (newDegree * PI) / 180.0f;
		float relativeY = radius * FMath::Sin(newDegreeInRadian) * inverseFactorY;
		float relativeZ = radius * FMath::Cos(newDegreeInRadian) * inverseFactorZ;
		//iconActor->SetActorRelativeLocation(FVector(0.0f, -1.0f*relativeY, -1.0f*relativeZ));

		float distDegree_modifiedForScale = (distDegree > 180.0f) ? (360.0f - distDegree) : distDegree;
		distDegree_modifiedForScale = (distDegree_modifiedForScale < -180.0f) ? (distDegree_modifiedForScale + 360.0f) : distDegree_modifiedForScale;
		float newTargetScale = maxScale * degreeScaleCurve->GetFloatValue(FMath::Abs(distDegree_modifiedForScale));

		FTransform curTr = iconActor->GetRootComponent()->GetRelativeTransform();
		FVector newPos = FMath::VInterpTo(curTr.GetLocation(), FVector(0.0f, -1.0f*relativeY, -1.0f*relativeZ), DeltaTime, effectInterSpeed);
		float newScale = FMath::FInterpTo(curTr.GetScale3D().X, newTargetScale, DeltaTime, effectInterSpeed);

		iconActor->SetActorRelativeTransform(FTransform(FQuat::Identity, newPos, FVector(newScale, newScale, newScale)));
	}
}

void ARingUIBase::ResetScalingEffect_Implementation(float DeltaTime)
{
	if (alreadyReset == false)
	{
		for (auto iconToDegree : iconToOriginalDegree)
		{
			auto iconActor = iconToDegree.Key;
			//if (IsValid(iconActor) == false) continue;
			float degreeInRadian = (iconToDegree.Value * PI) / 180.0f;
			float relativeY = radius * FMath::Sin(degreeInRadian) * inverseFactorY;
			float relativeZ = radius * FMath::Cos(degreeInRadian) * inverseFactorZ;
			//iconActor->SetActorRelativeLocation(FVector(0.0f, -1.0f*relativeY, -1.0f*relativeZ));
			//iconActor->SetActorRelativeTransform(FTransform(FQuat(), FVector(0.0f, -1.0f*relativeY, -1.0f*relativeZ), FVector(1.0f, 1.0f, 1.0f)));

			FTransform curTr = iconActor->GetRootComponent()->GetRelativeTransform();
			FVector newPos = FMath::VInterpTo(curTr.GetLocation(), FVector(0.0f, -1.0f*relativeY, -1.0f*relativeZ), DeltaTime, effectInterSpeed);
			float newScale = FMath::FInterpTo(curTr.GetScale3D().X, 1.0f, DeltaTime, effectInterSpeed);

			iconActor->SetActorRelativeTransform(FTransform(FQuat::Identity, newPos, FVector(newScale, newScale, newScale)));
		}
		//alreadyReset = true;
	}
}
