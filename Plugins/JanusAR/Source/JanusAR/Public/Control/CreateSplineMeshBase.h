// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineMeshComponent.h"
#include "CreateSplineMeshBase.generated.h"

UCLASS()
class JANUSAR_API ACreateSplineMeshBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACreateSplineMeshBase();

	UFUNCTION(BlueprintCallable)
	void AddSplineMeshComps(FString name, const TArray<USplineMeshComponent*>& comps);

	UFUNCTION(BlueprintPure)
	TArray<USplineMeshComponent*> GetSplineMeshComps(FString name);

	UFUNCTION(BlueprintCallable)
	void ClearSplineMeshComps(FString name);

protected:
	// Called when the game starts or when spawned
	//virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	TMap<FString, TArray<USplineMeshComponent*>> visualSplineMesh;
};
