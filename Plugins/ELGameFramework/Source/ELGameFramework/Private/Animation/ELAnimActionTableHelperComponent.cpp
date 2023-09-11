// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/ELAnimActionTableHelperComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogELGameFrameworkAnimation, Log, All)

// Sets default values for this component's properties
UELAnimActionTableHelperComponent::UELAnimActionTableHelperComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UELAnimActionTableHelperComponent::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(AnimActionDataTable))
	{
		TArray<FELAnimActionStruct*> OutAllRows;
		FString Context; // Used in error reporting.
		AnimActionDataTable->GetAllRows<FELAnimActionStruct>(Context, OutAllRows); // Populates the array with all the data from the CSV.
		for (FELAnimActionStruct* AnimActionDataRow : OutAllRows)
		{
			FString actionName = AnimActionDataRow->ActionName;
			UAnimSequence* animSequence = AnimActionDataRow->AnimSequence;
			if (IsValid(animSequence))
			{
				if (AnimActionDataMap.Contains(actionName) == false)
				{
					AnimActionDataMap.Add(actionName, TArray<UAnimSequence*>());
				}
				AnimActionDataMap[actionName].Add(animSequence);
			}
		}
	}
}

void UELAnimActionTableHelperComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	AnimActionDataMap.Empty();
}

UAnimSequence* UELAnimActionTableHelperComponent::GetAnimSequenceByAction(FString ActionName)
{
	if (AnimActionDataMap.Contains(ActionName))
	{
		int animSeqNum = AnimActionDataMap[ActionName].Num();
		UAnimSequence* animSequence = AnimActionDataMap[ActionName][FMath::RandHelper(animSeqNum)];
		return animSequence;
	}
	UE_LOG(LogELGameFrameworkAnimation, Warning, TEXT("[%s] No animation sequence found by Action Name: %s."), *AnimActionDataTable->GetName(), *ActionName);
	return nullptr;
}
