// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "WeatherSpawnerBase.generated.h"

UCLASS()
class JANUSAR_API AWeatherSpawnerBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeatherSpawnerBase();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UBoxComponent* Range = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		USphereComponent* Effector = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UParticleSystemComponent* FingerTrail = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UParticleSystemComponent* TrailDecoration = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UParticleSystemComponent* FarTrail = nullptr;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParticleEffect")
		float EffectRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParticleEffect")
		bool EnableParticleEffect;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "ParticleEffect")
		FVector EffectorPos;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Finger")
		FVector CurrentPos;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Finger")
		FVector LastPos;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Finger")
		FVector MiddlePos;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Finger")
		float CurrentSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Threshold")
		float FingerSpeedThreshold;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "State")
		bool IsUpdating;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
		bool FarMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
		bool HideFingerTrailWhenFarMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeatherParticle")
		bool EnableManualSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeatherParticle")
		float RandomDelayMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeatherParticle")
		float RandomDelayMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeatherParticle")
		float LocationRandomRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeatherParticle")
		float WeatherParticleWeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeatherParticle")
		TArray<UParticleSystem*> SpawnParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Threshold")
		float DistanceThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Size")
		float PlaneHeight;

	
	UPROPERTY(BlueprintReadWrite)
		FTransform particleSpawnTransform;

private:
	bool farTrailDoOnce = true;
	bool endFarTrailDoOnce = true;
	bool startUpdateDoOnce = true;
	bool tickGateBool = false;

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void OpenTickGate();

	UFUNCTION(BlueprintCallable)
		void CloseTickGate();

	//functions
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "WeatherParticle")
		UParticleSystem* GetSpawnParticle();

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "WeatherParticle")
		FVector GetSpawnLocation();

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "WeatherParticle")
		FRotator GetSpawnRotation();

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "FarMode")
		FVector GetFarModePos(FVector Pos);

	UFUNCTION(BlueprintNativeEvent, BlueprintPure)
		float GetCurrentFingerSpeed();

	//custom events   
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void OnStartUpdate();

	UFUNCTION(NetMulticast, Reliable)
		void OnStartUpdateMulticast();

	UFUNCTION(BlueprintImplementableEvent)
		void OnStartUpdateTrailMulticast();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void OnEndUpdate();

	UFUNCTION(NetMulticast, Reliable)
		void OnEndUpdateMulticast();

	UFUNCTION(BlueprintImplementableEvent)
		void OnEndUpdateTrailMulticast();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void UpdateCurFingerPos(bool UpdateLast);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void UpdateTrail();

	UFUNCTION(NetMulticast, Reliable)
		void UpdateTrailMulticast();

	UFUNCTION(BlueprintImplementableEvent)
		void OnUpdateTrailMulticast();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void UpdateParticle();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void UpdateParticleSpawn();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void SpawnWeatherParticle();

	UFUNCTION(NetMulticast, Reliable)
		void SpawnWeatherParticleMulticast();

	UFUNCTION(BlueprintImplementableEvent)
		void DebugMulticast();


};
