// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RingUIIconBase.h"
#include "Curves/CurveFloat.h"
#include "RingUIBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectedEnterDelegate, class ARingUIIconBase*, RingUIIcon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectedLeaveDelegate, class ARingUIIconBase*, RingUIIcon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectedDelegate, class ARingUIIconBase*, RingUIIcon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCancelSelectedDelegate, class ARingUIIconBase*, RingUIIcon);

UCLASS()
class JANUSAR_API ARingUIBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARingUIBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform & Transform) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION(BlueprintNativeEvent, Category = "RingUI")
	void UpdateScalingEffect(float DeltaTime);

	UFUNCTION(BlueprintNativeEvent, Category = "RingUI")
	void ResetScalingEffect(float DeltaTime);

	UFUNCTION(BlueprintPure, Category = "RingUI")
	ARingUIIconBase* GetCurrentSelected();


	UPROPERTY(BlueprintAssignable, Category = "RingUI")
	FOnSelectedEnterDelegate OnSelectedEnter;

	UPROPERTY(BlueprintAssignable, Category = "RingUI")
	FOnSelectedLeaveDelegate OnSelectedLeave;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "RingUI")
	FOnSelectedDelegate OnSelected;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "RingUI")
	FOnCancelSelectedDelegate OnCancelSelected;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RingUI")
	float radius = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RingUI")
	float scalingDegreeOffset = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RingUI")
	float startDegreeOffset = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RingUI")
	float maxScale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RingUI")
	TArray<ARingUIIconBase*> iconActors;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RingUI")
	UCurveFloat* degreeOffsetCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RingUI")
	UCurveFloat* degreeScaleCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RingUI")
	bool inverseY = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RingUI")
	bool inverseZ = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RingUI")
	float effectInterSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RingUI")
	float selectedDegreeThreshold = 10.0f;

protected:
	UFUNCTION(BlueprintSetter)
	void SetCurrentTouchDegree(float value);

	UFUNCTION(BlueprintSetter)
	void SetScalingEffectEnable(bool value);


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RingUI")
	TMap<ARingUIIconBase*, float> iconToOriginalDegree;

	UPROPERTY(VisibleAnywhere, BlueprintSetter=SetCurrentTouchDegree, Category = "RingUI")
	float curTouchDegree = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "RingUI")
	bool isTouch = false;

	UPROPERTY(VisibleAnywhere, BlueprintSetter=SetScalingEffectEnable, Category = "RingUI")
	bool scalingEffectEnable = false;


	float initDegreeOffset = 0.0f;
	bool alreadyReset = false;

	float inverseFactorY = 1.0f;
	float inverseFactorZ = 1.0f;

	ARingUIIconBase* currentSelected = nullptr;
};
