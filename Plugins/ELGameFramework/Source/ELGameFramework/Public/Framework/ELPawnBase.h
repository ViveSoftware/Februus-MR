// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ELPawnBase.generated.h"

UCLASS()
class ELGAMEFRAMEWORK_API AELPawnBase : public APawn
{
	GENERATED_BODY()

	// Begin Pawn overrides
	virtual UPawnMovementComponent* GetMovementComponent() const override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	// End Pawn overrides

public:
	// Sets default values for this pawn's properties
	AELPawnBase();

protected:
	/**
	* Input callback to move forward in local space (or backward if Val is negative).
	* @param Val Amount of movement in the forward direction (or backward if negative).
	* @see APawn::AddMovementInput()
	*/
	UFUNCTION(BlueprintCallable, Category = "VR_DesktopDebug")
	virtual void MoveForward(float Val);

	/**
	* Input callback to strafe right in local space (or left if Val is negative).
	* @param Val Amount of movement in the right direction (or left if negative).
	* @see APawn::AddMovementInput()
	*/
	UFUNCTION(BlueprintCallable, Category = "VR_DesktopDebug")
	virtual void MoveRight(float Val);

	/**
	* Input callback to move up in world space (or down if Val is negative).
	* @param Val Amount of movement in the world up direction (or down if negative).
	* @see APawn::AddMovementInput()
	*/
	UFUNCTION(BlueprintCallable, Category = "VR_DesktopDebug")
	virtual void MoveUp_World(float Val);

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	UFUNCTION(BlueprintCallable, Category = "VR_DesktopDebug")
	virtual void TurnAtRate(float Rate);

	/**
	* Called via input to look up at a given rate (or down if Rate is negative).
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	UFUNCTION(BlueprintCallable, Category = "VR_DesktopDebug")
	virtual void LookUpAtRate(float Rate);

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR_DesktopDebug")
	float BaseTurnRate = 45.f;

	/** Base lookup rate, in deg/sec. Other scaling may affect final lookup rate. */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR_DesktopDebug")
	float BaseLookUpRate = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR_DesktopDebug")
	float DebugMovementSpeed = 1.f;

	UPROPERTY(Category = "VR_DesktopDebug", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool enableDesktopDebugControl = false;

	/** DefaultPawn movement component */
	//UPROPERTY(Category = "VR_DesktopDebug", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	//UPawnMovementComponent* VRDebugMovementComponent;

	bool bELDebugPawnBindingsAdded = false;

public:

	/** If true, adds default input bindings for movement and camera look. */
	UPROPERTY(Category = "VR_DesktopDebug", EditAnywhere, BlueprintReadOnly)
	uint32 bAddDefaultMovementBindings : 1;

	//virtual void OnConstruction(const FTransform& Transform) override;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InitSpawn")
//	TArray<TSubclassOf<AActor>> SpawnClass;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InitSpawn")
//	TArray<AActor*> SpawnObjects;

private:
	void InitializeDebugELPawnInputBindings();
};
