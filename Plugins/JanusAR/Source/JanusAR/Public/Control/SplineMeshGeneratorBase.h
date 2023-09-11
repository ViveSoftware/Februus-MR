// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "SplineMeshGeneratorBase.generated.h"

UCLASS()
class JANUSAR_API ASplineMeshGeneratorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASplineMeshGeneratorBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "SplineMeshGenerator")
	void StartGenerateSplineMesh(USplineComponent* spline, bool isClosedLoop, bool instantMode = true, float pointDistance = 3.0f);

	UFUNCTION(BlueprintCallable, Category = "SplineMeshGenerator")
	void PauseGenerateSplineMesh();

	UFUNCTION(BlueprintCallable, Category = "SplineMeshGenerator")
	void ResumeGenerateSplineMesh();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "SplineMeshGenerator")
	void FinishGenerateSplineMesh(bool visualSplineMeshVisibility = false);

	UFUNCTION(BlueprintCallable, Category = "SplineMeshGenerator")
	void UpdateModifiedPoints(TArray<int> point_indexes);

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "SplineMeshGenerator")
	void GetCurrentPos(FVector &position, bool &isValid);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "SplineMeshGenerator")
	bool CheckPosAcceptable(FVector currentPosition, FVector lastPosition);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "SplineMeshGenerator")
	void AddRestOfSplineMesh_Server();

	UFUNCTION(BlueprintCallable, Category = "SplineMeshGenerator")
	void AddRestOfSplineMesh();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "SplineMeshGenerator")
	void ClearSplineAndSplineMesh_RPCMulticast();

	UFUNCTION(BlueprintCallable, Category = "SplineMeshGenerator")
	void ClearSplineMesh(TArray<int> point_indexes);

	UFUNCTION(BlueprintCallable, Category = "SplineMeshGenerator")
	void RegenerateSplineMesh(bool visibility);

	UFUNCTION(BlueprintImplementableEvent, Category = "SplineMeshGenerator")
	USplineMeshComponent* SetupOneSplineMeshComponent();

	UFUNCTION(NetMulticast, Reliable)
	void AddSplinePoint_RPCMulticast(FVector pos);

	UFUNCTION(NetMulticast, Reliable)
	void UpdateSplinePointPosition_RPCMulticast(int index, FVector pos);

	void CheckClientSpline();
	void SetSplineMeshStartAndEnd(int startIndex, int endIndex);
	void SetSplineMeshCloseLoop();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineMeshGenerator")
	USplineComponent *splineComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineMeshGenerator")
	TArray<USplineMeshComponent*> visualSplineMeshComponents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineMeshGenerator")
	bool instantAddPointMode = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineMeshGenerator")
	float delayForNotInstantMode = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineMeshGenerator")
	float pointDistanceThreshold = 3.0f;

	bool isFirstInstantPointAdded = false;
	bool isNewInstantPointAdded = false;

	FVector lastPos;
	int splineMeshCompCurrentLastIndex = 0;
	int splineMeshCompLastLastIndex = 0;

	bool isSplineClosedLoop;
};
