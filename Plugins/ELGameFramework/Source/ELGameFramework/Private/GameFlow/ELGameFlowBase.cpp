// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFlow/ELGameFlowBase.h"

// Sets default values
AELGameFlowBase::AELGameFlowBase()
{
	bReplicates = true;
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	nameText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("NameText"));
	nameText->SetupAttachment(RootComponent);
	nameText->SetHiddenInGame(true);
	nameText->SetTextRenderColor(FColor::Purple);
}

void AELGameFlowBase::EndFlow(FString specifyNextFlow)
{
	if (OnEndFlow.IsBound())
	{
		OnEndFlow.Execute(specifyNextFlow);
	}
	else
	{
		UE_LOG(LogELGameFramework, Warning, TEXT("[%s] End flow event is out of bound"), *this->GetName());
	}
	//OnEndFlow.Unbind();
}

void AELGameFlowBase::UpdateText(FText text)
{
	nameText->SetText(text);
}

void AELGameFlowBase::FastForward_Implementation(FString& specifyNextFlow)
{
	specifyNextFlow.Empty();
}

void AELGameFlowBase::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);
	if (name.IsEmpty())
	{
		name = GetClass()->GetName();
	}

	if (hasMgr == false)
	{
		FText tName = FText::FromString(name);
		//nameText->Text = tName;
		nameText->SetText(tName);
	}
}

