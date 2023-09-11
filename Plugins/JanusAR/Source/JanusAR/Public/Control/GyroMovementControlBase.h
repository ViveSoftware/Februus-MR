// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MotionControllerComponent.h"
#include "GyroMovementControlBase.generated.h"

UCLASS()
class JANUSAR_API AGyroMovementControlBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGyroMovementControlBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "GyroMovement")
	void SetupCurrentMotionControllerState();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GyroMovement")
	AActor* controlledActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GyroMovement")
	float moveRadius = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GyroMovement")
	float moveOriginThreshold = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GyroMovement")
	float moveOriginSpeed = 10.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GyroMovement")
	UMotionControllerComponent* motionControllerComp;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	FVector curPlaneNormal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	FVector curPlaneLocalX;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	FVector curPlaneLocalY;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	FVector curMCForward;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	FVector curMCForwardProjection;


	FVector originalControlledActorLocation;
};
