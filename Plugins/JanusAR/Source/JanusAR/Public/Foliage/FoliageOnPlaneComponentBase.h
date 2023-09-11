// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "FoliageOnPlaneComponentBase.generated.h"

USTRUCT(BlueprintType)
struct FFoliageGridCellStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Grid Cell Struct")
	FVector Center = FVector::Zero();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Grid Cell Struct")
	FVector TopLeft = FVector::Zero();;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Grid Cell Struct")
	FVector TopRight = FVector::Zero();;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Grid Cell Struct")
	bool isPlant = false;
};

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class JANUSAR_API UFoliageOnPlaneComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFoliageOnPlaneComponentBase();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void GenerateFoliage();

	UFUNCTION(BlueprintCallable)
	void SetupMaxNumOfFoliage();

	UFUNCTION(BlueprintCallable)
	void CreateGrids();

	UFUNCTION(BlueprintCallable)
	void GenerateFoliageInGrids();

	UFUNCTION(BlueprintCallable)
	void SpawnAllFoliageInstance();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool SpawnOneFoliageInstance(FTransform transform_);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void TracePlane(FVector start, FVector end, FHitResult& out_hit, bool& valid);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool GridIsPlant(FFoliageGridCellStruct grid);

protected:
	UFUNCTION(BlueprintSetter)
	void SetInstancedMeshComp(UInstancedStaticMeshComponent* comp);

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FoliageOnPlane", meta = (ClampMin = "0", UIMin = "0"))
	int priority = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FoliageOnPlane")
	UStaticMesh* foliageMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FoliageOnPlane", meta = (ClampMin = "0", UIMin = "0"))
	float boundScale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FoliageOnPlane", meta = (ClampMin = "0", UIMin = "0"))
	int numberOfFoliage = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FoliageOnPlane", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float foliageDensity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FoliageOnPlane", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float gridCellSize = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FoliageOnPlane", meta = (ClampMin = "0.0", ClampMax = "2.0", UIMin = "0.0", UIMax = "2.0"))
	float foliageScaleMax = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FoliageOnPlane", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float foliageScaleMin = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FoliageOnPlane", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float foliageRadius = 0.0f;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FoliageOnPlane", meta = (ClampMin = "0.0", ClampMax = "90.0", UIMin = "0.0", UIMax = "90.0"))
	float maxSlopeAngle = 50.0f;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "FoliageOnPlane")
	int rowsNum = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "FoliageOnPlane")
	int columnsNum = 0;

	UPROPERTY(BlueprintReadWrite, Category = "FoliageOnPlane")
	TArray<FFoliageGridCellStruct> Grids;

	UPROPERTY(BlueprintReadWrite, Category = "FoliageOnPlane")
	TArray<int> GridIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintSetter=SetInstancedMeshComp, Category = "FoliageOnPlane")
	UInstancedStaticMeshComponent* instancedMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "FoliageOnPlane")
	UPrimitiveComponent* planeMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FoliageOnPlane")
	int pickedFoliageInGridNum = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FoliageOnPlane")
	int actualFoliageInstanceNum = 0;

private:
	FVector planeMeshCenter;
	FVector planeMeshBoxExtent;
	float plaenMeshSphereRadius;
};
