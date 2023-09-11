// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Animation/AnimSequenceBase.h"
#include "Framework/ELSystemBase.h"
#include "ELGameFramework.h"
#include "ELSmartObjectBase.generated.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EELSmartObjectInterruptHandleSelfSetting : uint8
{
	VE_Default 				UMETA(DisplayName = "Default"),
	VE_Pause 				UMETA(DisplayName = "Pause"),
	VE_Leave 				UMETA(DisplayName = "Leave"),
	VE_NoInterrupt 			UMETA(DisplayName = "NoInterrupt"),
};

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EELSmartObjectInterruptHandleRequestByOther : uint8
{
	VE_Pause 				UMETA(DisplayName = "Pause"),
	VE_Leave 				UMETA(DisplayName = "Leave"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSmartObjectFinished, class AActor*, actor, class AELSmartObjectBase*, finishedSmartObject);

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class ELGAMEFRAMEWORK_API AELSmartObjectBase : public AELSystemBase
{
	GENERATED_BODY()
public:
	AELSmartObjectBase();

	void DoFinishSmartObject(AActor* actor);
	void PostFinishSmartObject(AActor* actor);
	EELSmartObjectInterruptHandleSelfSetting GetInterruptHandle() { return interruptHandle; };
	bool GetCanBeFound() { return canBeFound; };

	UPROPERTY(BlueprintAssignable, Category = "ELGameFramework | ELSmartObject")
	FOnSmartObjectFinished OnSmartObjectFinished;

	FOnSmartObjectFinished OnSmartObjectPreFinished;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ELGameFramework | ELSmartObject")
	bool CanActorJoin(AActor* actor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ELGameFramework | ELSmartObject")
	bool CanActorInterrupted(AActor* actor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ELGameFramework | ELSmartObject")
	int GetPriority(AActor* actor);

	UFUNCTION(BlueprintCallable, Category = "ELGameFramework | ELSmartObject")
	void ResetCoolDown(AActor* actor);

	UFUNCTION(BlueprintCallable, Category = "ELGameFramework | ELSmartObject")
	void PauseActor(AActor* actor);

	UFUNCTION(BlueprintCallable, Category = "ELGameFramework | ELSmartObject")
	void ResumeActor(AActor* actor);

	UFUNCTION(BlueprintImplementableEvent, Category = "ELGameFramework | ELSmartObject")
	void OnActorPuase(AActor* actor);

	UFUNCTION(BlueprintImplementableEvent, Category = "ELGameFramework | ELSmartObject")
	void OnActorResume(AActor* actor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ELGameFramework | ELSmartObject")
	void OnActorRecieveAnimNotify(AActor* actor, FName notifyName, USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);

	UFUNCTION(BlueprintCallable, Category = "ELGameFramework | ELSmartObject")
	void FinishSmartObject(AActor* actor);

	UFUNCTION(BlueprintCallable, Category = "ELGameFramework | ELSmartObject")
	void RunScenario(AActor* actor, UELScenarioComponentBase* scenarioTemplate, bool& success, UELScenarioComponentBase*& runnedScenario);

	UFUNCTION(BlueprintCallable, Category = "ELGameFramework | ELSmartObject")
	void PauseScenario(AActor* actor, bool& success, UELScenarioComponentBase*& pausedScenario);

	UFUNCTION(BlueprintCallable, Category = "ELGameFramework | ELSmartObject")
	void ResumeScenario(AActor* actor, bool& success, UELScenarioComponentBase*& resumedScenario);

	UFUNCTION(BlueprintCallable, Category = "ELGameFramework | ELSmartObject")
	void StopScenario(AActor* actor, bool& success, UELScenarioComponentBase*& stoppedScenario);

	UFUNCTION(BlueprintCallable, Category = "ELGameFramework | ELSmartObject")
	UELScenarioComponentBase* GetActorCurrentScenario(AActor* actor);

	UFUNCTION()
	void OnSenarioPreFinished(AActor* actor, UELScenarioComponentBase* scenario);

	//UFUNCTION(BlueprintImplementableEvent, Category = "ELGameFramework | ELSmartObject | Animation")
	//void OnRecieveAnimNotify(AActor* actor, FString NotifyName, USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ELGameFramework")
	USphereComponent* rangeSphere = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ELGameFramework")
	EELSmartObjectInterruptHandleSelfSetting interruptHandle = EELSmartObjectInterruptHandleSelfSetting::VE_Default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ELGameFramework")
	bool canBeFound = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ELGameFramework")
	bool rangeCheck = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ELGameFramework")
	int priority = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ELGameFramework")
	float coolDownForEachActor = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ELGameFramework")
	TArray<AActor*> pauseActors;

	TMap<AActor*, UELScenarioComponentBase*> actorToRunnedScenario;
	TMap<AActor*, UELScenarioComponentBase*> actorToPausedScenario;
	TMap<AActor*, float> actorToCoolDown;

private:
	virtual void DoAddActor(AActor* actor) override;
	virtual void DoRemoveActor(AActor* actor) override;
};
