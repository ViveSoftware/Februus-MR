// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TextRenderComponent.h"
#include "ELGameFlowBase.generated.h"

DECLARE_DELEGATE_OneParam(EndFlowDelegate, FString);

UCLASS()
class ELGAMEFRAMEWORK_API AELGameFlowBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AELGameFlowBase();

	UFUNCTION(BlueprintImplementableEvent, Category = "ELGameFramework")
	void StartFlow();

	UFUNCTION(BlueprintCallable, Category = "ELGameFramework | ELGameFlow")
	void EndFlow(FString specifyNextFlow);

	UFUNCTION(BlueprintNativeEvent, Category = "ELGameFramework")
	void FastForward(FString& specifyNextFlow);

	FString GetFlowName() { return name; };

	EndFlowDelegate OnEndFlow;

	void UpdateText(FText text);

	bool hasMgr = false;

protected:
	virtual void OnConstruction(const FTransform & Transform) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ELGameFramework")
	FString name;

	UTextRenderComponent* nameText;
};
