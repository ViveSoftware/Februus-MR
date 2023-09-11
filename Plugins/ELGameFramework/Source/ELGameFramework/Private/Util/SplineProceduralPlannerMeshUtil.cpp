// Fill out your copyright notice in the Description page of Project Settings.

#include "Util/SplineProceduralPlannerMeshUtil.h"
#include "ELGameFramework.h"

//DEFINE_LOG_CATEGORY_STATIC(LogELGameFramework, Log, All)

void USplineProceduralPlannerMeshUtil::MakePointGrid(USplineComponent * spline, float triangleSize, TArray<FVector> &vertices, TArray<int> &pointData, int &GridX, FVector &ZVector_CCWCW)
{
	if (triangleSize <= 0.0f) triangleSize = 5.0f;

	// determine cw or ccw
	int numOfSplinePoint = spline->GetNumberOfSplinePoints();
	FVector splineZeroLocation = spline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
	FVector splineFirstVector = spline->GetLocationAtSplinePoint(1, ESplineCoordinateSpace::World) - splineZeroLocation;
	FVector splineMidVector = spline->GetLocationAtSplinePoint(numOfSplinePoint*0.5f, ESplineCoordinateSpace::World) - splineZeroLocation;
	if (FVector::CrossProduct(splineFirstVector, splineMidVector).Z >= 0)
	{
		ZVector_CCWCW = FVector(0.0f, 0.0f, 1.0f);
	}
	else
	{
		ZVector_CCWCW = FVector(0.0f, 0.0f, -1.0f);
	}

	// spline center and bound
	FVector center, bound;
	float radius;
	GetSplineCenterAndBoundFromSplinePoints(spline, center, bound, radius);

	// grid x/y num
	float triangleSizeClamp = FMath::Clamp(triangleSize, 2.0f, 30.0f);
	int numX = FMath::CeilToInt((bound.X / triangleSizeClamp) + 1.0f);
	int numY = FMath::CeilToInt((bound.Y / (triangleSizeClamp * 0.5f * FMath::Tan(FMath::DegreesToRadians(60)) ) )+1.0f);
	GridX = numX * 2.0f;
	
	// setup vertices data
	int curPoint = -1;
	for (int indexY = -1*numY; indexY <= numY; indexY++)
	{
		for (int indexX = -1 * numX; indexX <= numX; indexX++)
		{
			float x = center.X + indexX*triangleSizeClamp + triangleSizeClamp*0.5f*(FMath::Abs(indexY+numY)%2);
			float y = center.Y + triangleSizeClamp*0.5f*FMath::Tan(FMath::DegreesToRadians(60))*indexY;

			FVector curLocation = FVector(x, y, center.Z);
			FVector curEdgeLocation = spline->FindLocationClosestToWorldLocation(curLocation, ESplineCoordinateSpace::World);
			if (curLocation.Z != curEdgeLocation.Z) curLocation.Z = curEdgeLocation.Z;
			FVector curDirection = curLocation - curEdgeLocation;
			FVector curEdgeDirection = FVector::CrossProduct(ZVector_CCWCW, spline->FindDirectionClosestToWorldLocation(curLocation, ESplineCoordinateSpace::World));
			
			bool isInside = FVector::DotProduct(curDirection, curEdgeDirection) > 0.0f;
			bool isEdge = curDirection.Size() < triangleSizeClamp;

			curPoint++;

			if (isInside)
			{
				vertices.Add(curLocation);
				pointData.Add(0);		// inside
			}
			else if (isEdge)
			{
				vertices.Add(curEdgeLocation);
				pointData.Add(1);		// edge
			}
			else
			{
				vertices.Add(curLocation);
				pointData.Add(-1);		// outside
			}
		}
	}

}

void USplineProceduralPlannerMeshUtil::BuildMeshFromSplineOutline(USplineComponent * spline, float triangleSize, TArray<FVector>& vertices, TArray<int>& triangles, TArray<FVector2D>& UV, TArray<FLinearColor>& vertexColor, TArray<int>& edge, FVector & ZVector_CCWCW)
{
	if (triangleSize <= 0.0f) triangleSize = 5.0f;

	TArray<int> pointData;
	int gridX;

	MakePointGrid(spline, triangleSize, vertices, pointData, gridX, ZVector_CCWCW);

	int point1, point2, point3;

	// triagles index
	for (int i = gridX + 1; i <= pointData.Num()-2; i++)
	{
		// inverted triangles
		point1 = ((((i / (gridX + 1)) % 2) * -1) + 1) + i;
		point3 = i - (gridX + 1);
		point2 = point3 - (-1);

		if (pointData[point1] != -1 && pointData[point2] != -1 && pointData[point3] != -1 && pointData[point1] + pointData[point2] + pointData[point3] != 3)
		{
			triangles.Add(point1);
			triangles.Add(point2);
			triangles.Add(point3);
		}

		// triangles
		point1 = i;
		point2 = i + 1;
		point3 = i - ((gridX + 1) - ((i / (gridX + 1)) % 2));

		if (pointData[point1] != -1 && pointData[point2] != -1 && pointData[point3] != -1 && pointData[point1] + pointData[point2] + pointData[point3] != 3)
		{
			triangles.Add(point1);
			triangles.Add(point2);
			triangles.Add(point3);
		}
	}

	// edge index
	for (int i = 0 ; i < pointData.Num(); i++)
	{
		if (pointData[i] == 1)
		{
			edge.Add(i);
		}
	}

	NormalizeVertexGridforUV(vertices, spline, 0.05, UV, vertexColor);
}

void USplineProceduralPlannerMeshUtil::NormalizeVertexGridforUV(TArray<FVector>& vertices, USplineComponent * spline, float padding, TArray<FVector2D>& UV, TArray<FLinearColor>& vertexColor)
{
	//UV.Empty(vertices.Num());
	UV.Init(FVector2D(), vertices.Num());
	//vertexColor.Empty(vertices.Num());
	vertexColor.Init(FLinearColor(), vertices.Num());

	// spline center and bound
	FVector center, bound;
	float radius;
	GetSplineCenterAndBoundFromSplinePoints(spline, center, bound, radius);

	float paddingClamp = FMath::Clamp(padding, 0.0f, 0.25f);
	float scale = bound.X > bound.Y ? (1.0f-paddingClamp) / (bound.X * 2.0f) : (1.0f-paddingClamp) / (bound.Y * 2.0f);
	FVector originalScale = scale * center;

	for (int i = 0; i < vertices.Num(); i++)
	{
		FVector curVertex = vertices[i];
		float UVx = curVertex.X*scale + 0.5 + -1.0f*originalScale.X;
		float UVy = curVertex.Y*scale + 0.5 + -1.0f*originalScale.Y;
		UV[i].X = UVx;
		UV[i].Y = UVy;

		FVector edgeToVertex = spline->FindLocationClosestToWorldLocation(curVertex, ESplineCoordinateSpace::World) - curVertex;
		float vertexColorValue = scale * edgeToVertex.Size();
		//vertexColor[i] = FLinearColor(vertexColorValue, vertexColorValue, vertexColorValue);
		vertexColor[i].R = vertexColor[i].G = vertexColor[i].B = vertexColorValue;

	}
}

void USplineProceduralPlannerMeshUtil::FilterSpline(USplineComponent * spline, TArray<float> filter)
{
	int filterSize = filter.Num();

	if (filterSize == 0)
	{
		UE_LOG(LogELGameFramework, Warning, TEXT("FilterSpline with empty filter."));
		return;
	}

	int filterHalfSize = filterSize / 2;
	int splinePointNum = spline->GetNumberOfSplinePoints();
	TArray<FVector> originalSplineLocations;
	originalSplineLocations.Init(FVector::ZeroVector, splinePointNum);
	for (int i = 0; i < splinePointNum; i++)
	{
		originalSplineLocations[i] = spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
	}

	float filterWeightSum = 0.0f;
	for (auto weight : filter)
	{
		filterWeightSum += weight;
	}

	int processIndex;
	FVector locationSumWithWeight;
	for (int curIndex = 0; curIndex < splinePointNum; curIndex++)
	{
		int curStartIndex = curIndex - filterHalfSize;
		locationSumWithWeight = FVector::ZeroVector;
		for (int i = 0; i < filterSize; i++)
		{
			processIndex = curStartIndex + i;
			processIndex = processIndex < 0 ? processIndex + splinePointNum : processIndex;
			processIndex = processIndex >= splinePointNum ? processIndex - splinePointNum : processIndex;

			locationSumWithWeight += originalSplineLocations[processIndex] * filter[i];
		}

		spline->SetLocationAtSplinePoint(curIndex, locationSumWithWeight / filterWeightSum, ESplineCoordinateSpace::World);
	}

	spline->UpdateSpline();
}

void USplineProceduralPlannerMeshUtil::GetSplineCenterAndBoundFromSplinePoints(USplineComponent* spline, FVector& center, FVector& bound, float& radius)
{
	FVector sum;
	FVector max(spline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World));
	FVector min(max);

	int splinePointNum = spline->GetNumberOfSplinePoints();

	if (splinePointNum <= 1)
	{
		center = FVector(0.0f, 0.0f, 0.0f);
		bound.X = 0.0f;
		bound.Y = 0.0f;
		bound.Z = 0.0f;
		radius = 0.0f;
		return;
	}

	for (int i = 0; i < splinePointNum; i++)
	{
		FVector pointLocation = spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
		sum += pointLocation;
		if (max.X < pointLocation.X) max.X = pointLocation.X;
		if (max.Y < pointLocation.Y) max.Y = pointLocation.Y;
		if (max.Z < pointLocation.Z) max.Z = pointLocation.Z;
		if (min.X > pointLocation.X) min.X = pointLocation.X;
		if (min.Y > pointLocation.Y) min.Y = pointLocation.Y;
		if (min.Z > pointLocation.Z) min.Z = pointLocation.Z;
	}

	center = sum / splinePointNum;
	bound.X = 0.5 * (max.X - min.X);
	bound.Y = 0.5 * (max.Y - min.Y);
	bound.Z = 0.5 * (max.Z - min.Z);
	radius = FMath::Max3(bound.X, bound.Y, bound.Z);
}

