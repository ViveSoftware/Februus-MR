// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RingUIIconBase.generated.h"

UCLASS()
class JANUSAR_API ARingUIIconBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARingUIIconBase();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "RingUI")
	void OnSelected();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "RingUI")
	void OnCancelSelected();

	UFUNCTION(BlueprintImplementableEvent, Category = "RingUI")
	void OnSelectedEnter();

	UFUNCTION(BlueprintImplementableEvent, Category = "RingUI")
	void OnSelectedLeave();
};
