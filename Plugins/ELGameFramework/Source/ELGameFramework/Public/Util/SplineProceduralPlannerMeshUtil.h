// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Components/SplineComponent.h"
#include "SplineProceduralPlannerMeshUtil.generated.h"

/**
 * 
 */
UCLASS()
class ELGAMEFRAMEWORK_API USplineProceduralPlannerMeshUtil : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, Category = "ELSplinePlannerMesh")
	static void MakePointGrid(USplineComponent* spline, float triangleSize, TArray<FVector> &vertices, TArray<int> &pointData, int &GridX, FVector &ZVector_CCWCW);

	UFUNCTION(BlueprintCallable, Category = "ELSplinePlannerMesh")
	static void BuildMeshFromSplineOutline(USplineComponent* spline, float triangleSize, TArray<FVector> &vertices, TArray<int> &triangles, TArray<FVector2D> &UV, TArray<FLinearColor> &vertexColor, TArray<int> &edge, FVector &ZVector_CCWCW);

	UFUNCTION(BlueprintCallable, Category = "ELSplinePlannerMesh")
	static void NormalizeVertexGridforUV(TArray<FVector> &vertices, USplineComponent* spline, float padding, TArray<FVector2D> &UV, TArray<FLinearColor> &vertexColor);

	UFUNCTION(BlueprintCallable, Category = "ELSplinePlannerMesh")
	static void FilterSpline(USplineComponent* spline, TArray<float> filter);

	UFUNCTION(BlueprintCallable, Category = "ELSplinePlannerMesh")
	static void GetSplineCenterAndBoundFromSplinePoints(USplineComponent* spline, FVector& center, FVector& bound, float& radius);
};

