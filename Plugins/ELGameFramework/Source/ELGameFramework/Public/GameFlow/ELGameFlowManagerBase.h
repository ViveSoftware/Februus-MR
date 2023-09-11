// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ELGameFlowBase.h"
#include "ELGameFlowManagerBase.generated.h"

UCLASS()
class ELGAMEFRAMEWORK_API AELGameFlowManagerBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AELGameFlowManagerBase();

	UFUNCTION(Server, Reliable, BlueprintCallable, WithValidation, Category = "ELGameFramework | ELGameFlow")
	void AddGameflow(AELGameFlowBase* gameflow);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "ELGameFramework | ELGameFlow")
	AELGameFlowBase* GetCurrentFlow();

	UFUNCTION(BlueprintCallable, Category = "ELGameFramework | ELGameFlow")
	void StartFlow();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform & Transform) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ELGameFramework")
	bool startOnBeginPlay = true;

	UPROPERTY(Replicated, EditInstanceOnly, BlueprintReadOnly, Category = "ELGameFramework")
	TArray<AELGameFlowBase*> gameflows;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "ELGameFramework", meta = (ToolTip = "specify a gameflow to start or leave empty to start from index 0."))
	AELGameFlowBase* startGameflow;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "ELGameFramework")
	AELGameFlowBase* fastForwardTo;

	UTextRenderComponent* nameText;
private:
	UFUNCTION()
	void OnGameFlowEnd(FString specifyNextFlow);

	void InitAndStartFlow();
	void StartNextFlow();
	void DoFastForward(AELGameFlowBase *startFlow);

	//UPROPERTY(Replicated)
	TMap<FString, AELGameFlowBase*> gameflowsMap;

	UPROPERTY(Replicated)
	int curFlowIdx = 0;

	TArray<AELGameFlowBase*> gameflows_cpy;
};
