// Fill out your copyright notice in the Description page of Project Settings.

#include "Foliage/FoliageOnPlaneComponentBase.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UFoliageOnPlaneComponentBase::UFoliageOnPlaneComponentBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	// ...
}


void UFoliageOnPlaneComponentBase::GenerateFoliage_Implementation()
{
	SetupMaxNumOfFoliage();
	CreateGrids();
	GenerateFoliageInGrids();
	SpawnAllFoliageInstance();
}

void UFoliageOnPlaneComponentBase::SetupMaxNumOfFoliage()
{
	UKismetSystemLibrary::GetComponentBounds(planeMesh, planeMeshCenter, planeMeshBoxExtent, plaenMeshSphereRadius);
	float columnNumf = planeMeshBoxExtent.X*2.0f*boundScale / gridCellSize;
	float rowNumf = planeMeshBoxExtent.Y*2.0f*boundScale / gridCellSize;
	columnsNum = FMath::FloorToInt(columnNumf);
	rowsNum = FMath::FloorToInt(rowNumf);
	int maxFoliage = FMath::FloorToInt(columnNumf*rowNumf);
	numberOfFoliage = FMath::FloorToInt(foliageDensity * FMath::Clamp(numberOfFoliage, 0, maxFoliage));
}

void UFoliageOnPlaneComponentBase::CreateGrids()
{
	for (int i = 0; i <= columnsNum; i++)
	{
		for (int j = 0; j <= rowsNum; j++)
		{
			float tlx = (planeMeshCenter.X + planeMeshBoxExtent.X * boundScale) - i * gridCellSize;
			float tly = (planeMeshCenter.Y + planeMeshBoxExtent.Y * boundScale) - j * gridCellSize;
			float halfGridSize = gridCellSize * 0.5;
			FFoliageGridCellStruct gridcell;
			gridcell.TopLeft = FVector(tlx, tly, planeMeshCenter.Z);
			gridcell.Center = gridcell.TopLeft - FVector(halfGridSize, halfGridSize, 0.0f);
			gridcell.TopRight = gridcell.Center - FVector(halfGridSize, halfGridSize, 0.0f);
			gridcell.isPlant = false;
			GridIndex.Add(Grids.Add(gridcell));
		}
	}
}

void UFoliageOnPlaneComponentBase::GenerateFoliageInGrids()
{
	int randomIndex, pickedIndex, pickTime;
	for (int i = 0; i < numberOfFoliage; i++)
	{
		pickTime = 0;
		do
		{
			randomIndex = FMath::RandRange(0, GridIndex.Num() - 1);
			pickedIndex = GridIndex[randomIndex];
			pickTime++;
		} while (GridIsPlant(Grids[pickedIndex]) == false && pickTime <= 100);

		if (pickTime >= 100) continue;

		Grids[pickedIndex].isPlant = true;
		GridIndex.RemoveAt(randomIndex);
		pickedFoliageInGridNum++;
	}
}

void UFoliageOnPlaneComponentBase::SpawnAllFoliageInstance()
{
	float safeDistance = (gridCellSize - foliageRadius)*0.5f;
	for (auto grid : Grids)
	{
		if (grid.isPlant == false) continue;

		
		FVector spawnPos;
		FHitResult hitResult;
		bool hitValid;
		int traceTime = 0;
		FVector traceDist = FVector(0.0f, 0.0f, (planeMeshBoxExtent.Z + 10.0f)*1.5f);

		do
		{
			spawnPos = FVector((grid.Center.X + FMath::FRandRange(-1.0f*safeDistance, safeDistance)), (grid.Center.Y + FMath::FRandRange(-1.0f*safeDistance, safeDistance)), grid.Center.Z);
			TracePlane(spawnPos + traceDist, spawnPos - traceDist, hitResult, hitValid);
			traceTime++;
		} while (!hitValid && traceTime <= 100);

		if (hitValid)
		{
			FRotator rot = FRotator::ZeroRotator;
			rot.Yaw = FMath::FRandRange(0.0f, 360.f);
			float scaleValue = FMath::FRandRange(foliageScaleMin, foliageScaleMax);
			FVector scale = FVector(scaleValue, scaleValue, scaleValue);
			//int instIndex = instancedMeshComponent->AddInstance(FTransform(rot, hitResult.Location - ownerLocation, scale));
			if (SpawnOneFoliageInstance(FTransform(rot, hitResult.Location, scale))) actualFoliageInstanceNum++;
		}
	}
}

bool UFoliageOnPlaneComponentBase::SpawnOneFoliageInstance_Implementation(FTransform transform_)
{
	FVector ownerLocation = instancedMeshComponent->GetComponentLocation();
	FVector spawnLocation = transform_.GetLocation() - ownerLocation;
	int instIndex = instancedMeshComponent->AddInstance(FTransform(transform_.GetRotation(), spawnLocation, transform_.GetScale3D()));
	return true;
}

void UFoliageOnPlaneComponentBase::SetInstancedMeshComp(UInstancedStaticMeshComponent * comp)
{
	if (IsValid(comp))
	{
		instancedMeshComponent = comp;
		instancedMeshComponent->SetStaticMesh(foliageMesh);
	}
}

bool UFoliageOnPlaneComponentBase::GridIsPlant_Implementation(FFoliageGridCellStruct grid)
{
	return true;
}

