// Fill out your copyright notice in the Description page of Project Settings.

#include "Control/SplineMeshGeneratorBase.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASplineMeshGeneratorBase::ASplineMeshGeneratorBase()
{
	bReplicates = true;
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

//void ASplineMeshGeneratorBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//	DOREPLIFETIME(ASplineMeshGeneratorBase, splineComponent);
//}

// Called when the game starts or when spawned
void ASplineMeshGeneratorBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASplineMeshGeneratorBase::StartGenerateSplineMesh(USplineComponent* spline, bool isClosedLoop, bool instantMode, float pointDistance)
{
	splineComponent = spline;
	isSplineClosedLoop = isClosedLoop;
	instantAddPointMode = instantMode;
	pointDistanceThreshold = pointDistance;
	ClearSplineAndSplineMesh_RPCMulticast();
	SetActorTickEnabled(true);
}

void ASplineMeshGeneratorBase::PauseGenerateSplineMesh()
{
	SetActorTickEnabled(false);
}

void ASplineMeshGeneratorBase::ResumeGenerateSplineMesh()
{
	if (IsValid(splineComponent)) SetActorTickEnabled(true);
}

void ASplineMeshGeneratorBase::FinishGenerateSplineMesh_Implementation(bool visualSplineMeshVisibility)
{
	SetActorTickEnabled(false);
	if (IsValid(splineComponent))
	{
		AddRestOfSplineMesh();
		if (isSplineClosedLoop)
		{
			splineComponent->SetClosedLoop(true);
			SetSplineMeshCloseLoop();
		}
		for (auto splineMesh : visualSplineMeshComponents)
		{
			splineMesh->SetVisibility(visualSplineMeshVisibility);
		}
	}
}

void ASplineMeshGeneratorBase::UpdateModifiedPoints(TArray<int> point_indexes)
{
	if (point_indexes.Num() <= 0) return;

	TArray<int> sortedIndex = point_indexes;
	sortedIndex.Sort([](const int32 A, const int32 B)
	{
		return A < B;
	});
	
	SetSplineMeshStartAndEnd(sortedIndex[0] - 2, sortedIndex[sortedIndex.Num()-1] + 2);
	if (isSplineClosedLoop) SetSplineMeshCloseLoop();
}

void ASplineMeshGeneratorBase::GetCurrentPos_Implementation(FVector & position, bool & isValid)
{
	position = FVector();
	isValid = false;
}


bool ASplineMeshGeneratorBase::CheckPosAcceptable_Implementation(FVector currentPosition, FVector lastPosition)
{
	return ((currentPosition - lastPosition).SizeSquared() > pointDistanceThreshold*pointDistanceThreshold);
}

void ASplineMeshGeneratorBase::AddRestOfSplineMesh_Server_Implementation()
{
	AddRestOfSplineMesh();
}

void ASplineMeshGeneratorBase::AddRestOfSplineMesh()
{
	if (IsValid(splineComponent) == true)
	{
		//UE_LOG(LogTemp, Log, TEXT("[AddRestOfSplineMesh]"));
		int splinePointNum = splineComponent->GetNumberOfSplinePoints();
		if (splinePointNum >= 2 && (splinePointNum - 1) != splineMeshCompLastLastIndex)
		{
			splineMeshCompCurrentLastIndex = splinePointNum - 2;
			for (int i = splineMeshCompLastLastIndex; i <= splineMeshCompCurrentLastIndex; i++)
			{
				//USplineMeshComponent* splineMesh = NewObject<USplineMeshComponent>(splineComponent->GetOwner(), USplineMeshComponent::StaticClass());
				//splineMesh->RegisterComponent();
				//splineMesh->Mobility = EComponentMobility::Movable;
				//splineMesh->AttachToComponent(splineComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, false));
				visualSplineMeshComponents.Add(SetupOneSplineMeshComponent());
				SetSplineMeshStartAndEnd(i - 1, i + 1);
			}
			splineMeshCompLastLastIndex = splineMeshCompCurrentLastIndex + 1;
		}
	}
}

void ASplineMeshGeneratorBase::ClearSplineAndSplineMesh_RPCMulticast_Implementation()
{
	if (IsValid(splineComponent) == true)
	{
		splineComponent->ClearSplinePoints();
		splineComponent->SetClosedLoop(false);
	}
	else
	{
		CheckClientSpline();
	}

	for (auto splineMesh : visualSplineMeshComponents)
	{
		splineMesh->DestroyComponent();
	}
	visualSplineMeshComponents.Empty();

	isFirstInstantPointAdded = false;
	isNewInstantPointAdded = false;

	splineMeshCompCurrentLastIndex = 0;
	splineMeshCompLastLastIndex = 0;
}

void ASplineMeshGeneratorBase::ClearSplineMesh(TArray<int> point_indexes)
{
	if (point_indexes.Num() == 0)
	{
		for (auto splineMesh : visualSplineMeshComponents)
		{
			splineMesh->DestroyComponent();
		}
		visualSplineMeshComponents.Empty();
		UE_LOG(LogTemp, Log, TEXT("Clear all spline mesh"));
	}
	else
	{
		TArray<int> sortedIndex = point_indexes;
		sortedIndex.Sort([](const int32 A, const int32 B)
		{
			return A > B;
		});
		for (auto index : sortedIndex)
		{
			if (index >= visualSplineMeshComponents.Num()) continue;
			visualSplineMeshComponents[index]->DestroyComponent();
			visualSplineMeshComponents.RemoveAt(index);
		}
		UE_LOG(LogTemp, Log, TEXT("Clear spline mesh with index"));
	}
}

void ASplineMeshGeneratorBase::RegenerateSplineMesh(bool visibility)
{
	if (IsValid(splineComponent) == true)
	{
		ClearSplineMesh(TArray<int>());
		int splinePointNum = splineComponent->GetNumberOfSplinePoints();
		for (int i = 0; i <= splinePointNum - 2; i++)
		{
			auto splineMesh = SetupOneSplineMeshComponent();
			visualSplineMeshComponents.Add(splineMesh);
			FVector startPos, startTangent, endPos, endTangent;
			splineComponent->GetLocationAndTangentAtSplinePoint(i, startPos, startTangent, ESplineCoordinateSpace::World);
			splineComponent->GetLocationAndTangentAtSplinePoint(i + 1, endPos, endTangent, ESplineCoordinateSpace::World);
			splineMesh->SetStartAndEnd(startPos, startTangent, endPos, endTangent);
			splineMesh->SetVisibility(visibility);
		}
		if (isSplineClosedLoop && splinePointNum >= 3)
		{
			//auto splineMesh = SetupOneSplineMeshComponent();
			//visualSplineMeshComponents.Add(splineMesh);
			//FVector startPos, startTangent, endPos, endTangent;
			//splineComponent->GetLocationAndTangentAtSplinePoint(splinePointNum-1, startPos, startTangent, ESplineCoordinateSpace::World);
			//splineComponent->GetLocationAndTangentAtSplinePoint(0, endPos, endTangent, ESplineCoordinateSpace::World);
			//splineMesh->SetStartAndEnd(startPos, startTangent, endPos, endTangent);
			//splineMesh->SetVisibility(visibility);
			SetSplineMeshCloseLoop();
			visualSplineMeshComponents[splinePointNum - 1]->SetVisibility(visibility);
		}
	}
}

void ASplineMeshGeneratorBase::AddSplinePoint_RPCMulticast_Implementation(FVector pos)
{
	CheckClientSpline();
	splineComponent->AddSplinePoint(pos, ESplineCoordinateSpace::World);
	//UE_LOG(LogTemp, Log, TEXT("[AddSplinePoint_RPCMulticast_Implementation] Add point to spline component."));
}

void ASplineMeshGeneratorBase::UpdateSplinePointPosition_RPCMulticast_Implementation(int index, FVector pos)
{
	CheckClientSpline();
	if (splineComponent->GetNumberOfSplinePoints() < 1) return;
	splineComponent->SetLocationAtSplinePoint(index, pos, ESplineCoordinateSpace::World);
	//UE_LOG(LogTemp, Log, TEXT("[UpdateSplinePointPosition_RPCMulticast_Implementation]"));
}

void ASplineMeshGeneratorBase::CheckClientSpline()
{
	if (GetWorld()->IsNetMode(ENetMode::NM_Standalone) || GetWorld()->IsNetMode(ENetMode::NM_ListenServer))
	{
		//UE_LOG(LogTemp, Log, TEXT("[CheckClientSpline] Server need not check spline component."));
		return;
	}
	if (IsValid(splineComponent) == false)
	{
		UE_LOG(LogTemp, Log, TEXT("[CheckClientSpline] Client add spline component."));
		USplineComponent* tmp = NewObject<USplineComponent>(this);
		tmp->RegisterComponent();
		tmp->ClearSplinePoints();
		tmp->SetClosedLoop(false);
		splineComponent = tmp;
	}
}

void ASplineMeshGeneratorBase::SetSplineMeshStartAndEnd(int startIndex, int endIndex)
{
	if (IsValid(splineComponent) == true)
	{
		int splinePointNum = splineComponent->GetNumberOfSplinePoints();
		int startIndex_ = FMath::Clamp(startIndex, 0, splinePointNum - 2);
		int endIndex_ = FMath::Clamp(endIndex, 0, FMath::Min(splinePointNum - 2, visualSplineMeshComponents.Num()));
		for (int i = startIndex_; i <= endIndex_; i++)
		{
			auto splineMesh = visualSplineMeshComponents[i];
			if (IsValid(splineMesh))
			{
				FVector startPos, startTangent, endPos, endTangent;
				splineComponent->GetLocationAndTangentAtSplinePoint(i, startPos, startTangent, ESplineCoordinateSpace::World);
				splineComponent->GetLocationAndTangentAtSplinePoint(i+1, endPos, endTangent, ESplineCoordinateSpace::World);
				splineMesh->SetStartAndEnd(startPos, startTangent, endPos, endTangent);
			}
		}
	}
}

void ASplineMeshGeneratorBase::SetSplineMeshCloseLoop()
{
	if (IsValid(splineComponent) == true)
	{
		int splinePointNum = splineComponent->GetNumberOfSplinePoints();
		if (splinePointNum >= 3)
		{
			if (visualSplineMeshComponents.Num() == splinePointNum-1)
			{
				//USplineMeshComponent* splineMesh = NewObject<USplineMeshComponent>(splineComponent->GetOwner(), USplineMeshComponent::StaticClass());
				//splineMesh->RegisterComponent();
				//splineMesh->Mobility = EComponentMobility::Movable;
				//splineMesh->AttachToComponent(splineComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, false));
				visualSplineMeshComponents.Add(SetupOneSplineMeshComponent());
			}

			if (visualSplineMeshComponents.Num() == splinePointNum)
			{
				USplineMeshComponent* splineMesh = visualSplineMeshComponents[splinePointNum - 1];
				FVector startPos, startTangent, endPos, endTangent;
				splineComponent->GetLocationAndTangentAtSplinePoint(splinePointNum - 1, startPos, startTangent, ESplineCoordinateSpace::World);
				splineComponent->GetLocationAndTangentAtSplinePoint(0, endPos, endTangent, ESplineCoordinateSpace::World);
				splineMesh->SetStartAndEnd(startPos, startTangent, endPos, endTangent);
				SetSplineMeshStartAndEnd(0, 1);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Spline mesh not match when setting close loop"));
			}
		}
	}
}

// Called every frame
void ASplineMeshGeneratorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsValid(splineComponent) == true)
	{
		FVector curPos;
		bool curPosValid;
		GetCurrentPos(curPos, curPosValid);
		if (curPosValid == true)
		{
			// add new point
			if (instantAddPointMode)
			{
				// add fisrt point
				if (isFirstInstantPointAdded == false)
				{
					//splineComponent->AddSplinePoint(curPos, ESplineCoordinateSpace::World);
					AddSplinePoint_RPCMulticast(curPos);
					isFirstInstantPointAdded = true;
				}

				// add instant point
				if (isNewInstantPointAdded == false)
				{
					//splineComponent->AddSplinePoint(curPos, ESplineCoordinateSpace::World);
					AddSplinePoint_RPCMulticast(curPos);
					isNewInstantPointAdded = true;
					lastPos = curPos;
				}

				// update instant point position
				int lastPointIndex = splineComponent->GetNumberOfSplinePoints() - 1;
				//splineComponent->SetLocationAtSplinePoint(lastPointIndex, curPos, ESplineCoordinateSpace::World);
				UpdateSplinePointPosition_RPCMulticast(lastPointIndex, curPos);

				// instant point accepted, add new point next time
				if (CheckPosAcceptable(curPos, lastPos) == true)
				{
					isNewInstantPointAdded = false;
				}
			}
			else
			{
				if (isFirstInstantPointAdded == false)
				{
					splineComponent->AddSplinePoint(curPos, ESplineCoordinateSpace::World);
					lastPos = curPos;
					isFirstInstantPointAdded = true;
				}
				if (CheckPosAcceptable(curPos, lastPos) == true)
				{
					splineComponent->AddSplinePoint(curPos, ESplineCoordinateSpace::World);
					lastPos = curPos;
				}
			}

			// add spline mesh
			if (instantAddPointMode)
			{
				AddRestOfSplineMesh_Server();
				SetSplineMeshStartAndEnd(splineMeshCompLastLastIndex - 1, splineMeshCompLastLastIndex);
			}
			else
			{
				static double lastTime = 0;
				if (FPlatformTime::Seconds() - lastTime > delayForNotInstantMode)
				{
					AddRestOfSplineMesh_Server();
					lastTime = FPlatformTime::Seconds();
				}
			}
		}
	}
}

