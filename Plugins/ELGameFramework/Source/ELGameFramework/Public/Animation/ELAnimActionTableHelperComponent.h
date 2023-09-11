// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Animation/AnimSequence.h"
#include "Components/ActorComponent.h"
#include "ELAnimActionTableHelperComponent.generated.h"

/** Structure that defines action to animation sequence */
USTRUCT(BlueprintType)
struct FELAnimActionStruct : public FTableRowBase
{
	GENERATED_BODY()

public:

	FELAnimActionStruct()
		: ActionName("")
		, AnimSequence(nullptr)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Action Table")
	FString ActionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Action Table")
	UAnimSequence* AnimSequence;

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ELGAMEFRAMEWORK_API UELAnimActionTableHelperComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UELAnimActionTableHelperComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	TMap<FString, TArray<UAnimSequence*>> AnimActionDataMap;

public:
	UFUNCTION(BlueprintPure, Category = "Anim Action Table")
	UAnimSequence* GetAnimSequenceByAction(FString ActionName);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anim Action Table")
	UDataTable* AnimActionDataTable;

		
};
