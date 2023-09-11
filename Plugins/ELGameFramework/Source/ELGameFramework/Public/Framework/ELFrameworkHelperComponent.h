// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ELWorldBase.h"
#include "ELFrameworkHelperComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ELGAMEFRAMEWORK_API UELFrameworkHelperComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UELFrameworkHelperComponent();

	UPROPERTY(BlueprintReadOnly, Category = "ELGameFramework | PawnHelper")
	TArray<AELWorldBase* > GameWorlds;

	UPROPERTY(BlueprintReadOnly, Category = "ELGameFramework | PawnHelper")
	TMap<FString, AELWorldBase* > GameWorldsMap;

	UFUNCTION(BlueprintPure, Category = "ELGameFramework | PawnHelper")
	AELWorldBase* GetGameWorld(FString name);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void PostInitProperties() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void ConstructWorldMap();

		
};
