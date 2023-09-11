// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "EditableSplinePlaneActor.generated.h"

UCLASS()
class JANUSAR_API AEditableSplinePlaneActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEditableSplinePlaneActor();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void GeneratePlaneObjects();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateSplineLocation(int point_index, FVector location, ESplineCoordinateSpace::Type coordinate_space);

protected:
	// Called when the game starts or when spawned
	//virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Editable Plane Spline")
	USplineComponent *spline;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

};
