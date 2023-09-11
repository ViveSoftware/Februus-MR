// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Scene/EditableSplinePlaneActor.h"
#include "Curves/CurveFloat.h"
#include "EditableSplinePlaneModifier.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSplineUpdated, class USplineComponent*, spline, const TArray<int>&, updatedPoints);

UCLASS()
class JANUSAR_API AEditableSplinePlaneModifier : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEditableSplinePlaneModifier();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StartModifiying(USplineComponent *spline_, FVector startLocation_, AEditableSplinePlaneActor* editable_spline_plane_actor = nullptr);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateModifiying(FVector location);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EndModifiying();

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnSplineUpdated OnSplineModifyUpdated;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnSplineUpdated OnSplineModifyFinished;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetupOriginalSpline();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetupCenterSplinePointInfo();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CalculateAllSplinePointDistance();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateModifyingSplinePoints();

protected:
	// Called when the game starts or when spawned
	//virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EditableSplinePlaneModifier")
	USplineComponent* originalSpline = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EditableSplinePlaneModifier")
	USplineComponent* targetSpline = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EditableSplinePlaneModifier")
	FVector startLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EditableSplinePlaneModifier")
	FVector currentLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EditableSplinePlaneModifier")
	UCurveFloat* locationRatioCurve = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EditableSplinePlaneModifier")
	float centerDistance = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EditableSplinePlaneModifier")
	float originalRange = 10.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EditableSplinePlaneModifier")
	float currentRange = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EditableSplinePlaneModifier")
	float rangeFactor = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EditableSplinePlaneModifier")
	bool updateStartLocation = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EditableSplinePlaneModifier")
	bool specifiedDirection = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EditableSplinePlaneModifier")
	FVector modifiedDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EditableSplinePlaneModifier")
	AEditableSplinePlaneActor* targetEditableSplinePlaneActor = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EditableSplinePlaneModifier")
	TArray<FVector> splinePointOriginalLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EditableSplinePlaneModifier")
	TArray<float> splinePointDistanceToModifyingCenter;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EditableSplinePlaneModifier")
	TArray<int> modifyingSplinePoints;
};
