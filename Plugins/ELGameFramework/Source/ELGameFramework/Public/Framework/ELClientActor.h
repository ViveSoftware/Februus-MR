// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ELClientActor.generated.h"

UCLASS()
class ELGAMEFRAMEWORK_API AELClientActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AELClientActor();

	virtual void PostInitializeComponents() override;

};
