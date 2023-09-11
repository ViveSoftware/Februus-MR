// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Animation/AnimSequenceBase.h"
#include "ELScenarioComponentBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScenarioFinished, class AActor*, actor, class UELScenarioComponentBase*, finishedScenario);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ELGAMEFRAMEWORK_API UELScenarioComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UELScenarioComponentBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//virtual bool IsSupportedForNetworking() const override
	//{
	//	return true;
	//}

	UFUNCTION(BlueprintNativeEvent, Category = "ELGameFramework")
	bool Setup(UELScenarioComponentBase* referenceScenario);

	UFUNCTION(BlueprintNativeEvent, Category = "ELGameFramework")
	bool Start();

	UFUNCTION(BlueprintNativeEvent, Category = "ELGameFramework")
	bool Pause();

	UFUNCTION(BlueprintNativeEvent, Category = "ELGameFramework")
	bool Resume();

	UFUNCTION(BlueprintNativeEvent, Category = "ELGameFramework")
	bool Stop();

	UFUNCTION(BlueprintCallable, Category = "ELGameFramework | ELScenario")
	void FinishScenario();

	UFUNCTION(BlueprintNativeEvent, Category = "ELGameFramework")
	FString GetScenarioName();

	UPROPERTY(BlueprintAssignable, Category = "ELGameFramework | ELScenario")
	FOnScenarioFinished OnScenarioFinished;

	FOnScenarioFinished OnScenarioPreFinished;

	void PostStart();
	void PreStop();
	void PostStop();
	void PostPause();
	void PostResume();

	UFUNCTION(BlueprintPure, Category = "ELGameFramework | ELScenario")
	bool IsPause();

	UFUNCTION(BlueprintPure, Category = "ELGameFramework | ELScenario")
	bool IsRunning();

	UFUNCTION(BlueprintImplementableEvent, Category = "ELGameFramework | ELSmartObject")
	void RecieveAnimNotify(FName notifyName, USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ELGameFramework")
	FString name;

	UPROPERTY(Replicated, VisibleAnywhere, Category = "ELGameFramework | ELScenario")
	bool isPausing_ = false;

	UPROPERTY(Replicated, VisibleAnywhere, Category = "ELGameFramework | ELScenario")
	bool isRunning_ = false;

	bool isPreStop_ = false;

		
};
