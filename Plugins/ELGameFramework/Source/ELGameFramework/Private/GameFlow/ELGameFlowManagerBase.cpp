// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFlow/ELGameFlowManagerBase.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogELGameFlow, Log, All)

// Sets default values
AELGameFlowManagerBase::AELGameFlowManagerBase()
{
	bReplicates = true;
	//bOnlyRelevantToOwner = true;

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	nameText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("NameText"));
	nameText->SetupAttachment(RootComponent);
	nameText->SetHiddenInGame(true);
	nameText->SetTextRenderColor(FColor::Red);
}

AELGameFlowBase* AELGameFlowManagerBase::GetCurrentFlow()
{
	if (curFlowIdx < gameflows.Num())
	{
		return gameflows[curFlowIdx];
	}
	return nullptr;
}

void AELGameFlowManagerBase::StartFlow()
{
	InitAndStartFlow();
}

void AELGameFlowManagerBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AELGameFlowManagerBase, gameflows);
	//DOREPLIFETIME(AELGameFlowManagerBase, gameflowsMap);
	DOREPLIFETIME(AELGameFlowManagerBase, curFlowIdx);
}

bool AELGameFlowManagerBase::AddGameflow_Validate(AELGameFlowBase* gameflow)
{
	return gameflow != nullptr;
}

void AELGameFlowManagerBase::AddGameflow_Implementation(AELGameFlowBase* gameflow)
{
	FString flowName = gameflow->GetFlowName();
	if (gameflowsMap.Contains(flowName))
	{
		UE_LOG(LogELGameFlow, Warning, TEXT("[%s] ELGameFlow-%s's name \"%s\" is already exist."), *this->GetName(), *gameflow->GetName(), *flowName);
	}
	else
	{
		gameflows.Add(gameflow);
		gameflowsMap.Add(flowName, gameflow);
	}
}

// Called when the game starts or when spawned
void AELGameFlowManagerBase::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		for (AELGameFlowBase* flow : gameflows)
		{
			FString flowName = flow->GetFlowName();
			if (!flowName.IsEmpty())
			{
				if (gameflowsMap.Contains(flowName))
				{
					UE_LOG(LogELGameFlow, Warning, TEXT("[%s] ELGameFlow-%s's name \"%s\" is already exist."), *this->GetName(), *flow->GetName(), *flowName);
					continue;
				}
				gameflowsMap.Add(flowName, flow);
			}
		}

		if (gameflowsMap.Num() == 0)
		{
			UE_LOG(LogELGameFlow, Warning, TEXT("[%s] No gameflows."), *this->GetName());
			return;
		}

		if(startOnBeginPlay) InitAndStartFlow();
	}
}

void AELGameFlowManagerBase::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);

#if WITH_EDITOR
	FString name = this->GetActorLabel();
	FText tName = FText::FromString(name);
	nameText->SetText(tName);

	for (AELGameFlowBase* flow : gameflows_cpy)
	{
		if (IsValid(flow))
		{
			flow->hasMgr = false;
		}
	}

	AELGameFlowBase* tempStartFlow = startGameflow;
	if (tempStartFlow == nullptr && gameflows.Num() > 0)
	{
		tempStartFlow = gameflows[0];
	}
	for (int i = 0; i < gameflows.Num(); i++)
	{
		AELGameFlowBase* flow = gameflows[i];
		if (IsValid(flow))
		{
			FString flowString = FString::FromInt(i) + " " + flow->GetFlowName();
			if (tempStartFlow == flow) flowString = ">" + flowString;
			if (fastForwardTo == flow) flowString = ">>>>" + flowString;
			FText flowText = FText::FromString(flowString);
			flow->UpdateText(flowText);

			flow->SetActorLocationAndRotation(this->GetActorLocation(), this->GetActorRotation());
			flow->AddActorLocalOffset(FVector(0.0f, 0.0f, (i + 1)*-30.0f));
			flow->hasMgr = true;
		}
	}

	gameflows_cpy = gameflows;
#endif
}

void AELGameFlowManagerBase::OnGameFlowEnd(FString specifyNextFlow)
{
	AELGameFlowBase* curFlow = GetCurrentFlow();
	if (IsValid(curFlow))
	{
		curFlow->OnEndFlow.Unbind();
	}

	if (specifyNextFlow.IsEmpty())
	{
		StartNextFlow();
	}
	else
	{
		AELGameFlowBase* specifyFlow = gameflowsMap[specifyNextFlow];
		if (specifyFlow == nullptr)
		{
			StartNextFlow();
		}
		else
		{
			UE_LOG(LogELGameFlow, Log, TEXT("[%s] Start game flow: %s."), *this->GetName(), *specifyFlow->GetFlowName());
			curFlowIdx = gameflows.Find(specifyFlow);
			specifyFlow->OnEndFlow.BindUObject(this, &AELGameFlowManagerBase::OnGameFlowEnd);
			specifyFlow->StartFlow();
		}
	}
}

void AELGameFlowManagerBase::InitAndStartFlow()
{
	AELGameFlowBase* startFLow;
	if (startGameflow != nullptr && gameflows.Contains(startGameflow))
	{
		startFLow = startGameflow;
		curFlowIdx = gameflows.Find(startFLow);
	}
	else
	{
		startFLow = gameflows[0];
		curFlowIdx = 0;
	}

	if (fastForwardTo != nullptr)
	{
		DoFastForward(startFLow);
	}
	else
	{
		UE_LOG(LogELGameFlow, Log, TEXT("[%s] Start game flow: %s."), *this->GetName(), *startFLow->GetFlowName());
		startFLow->OnEndFlow.BindUObject(this, &AELGameFlowManagerBase::OnGameFlowEnd);
		startFLow->StartFlow();
	}
}

void AELGameFlowManagerBase::StartNextFlow()
{
	curFlowIdx++;
	if (curFlowIdx < gameflows.Num())
	{
		UE_LOG(LogELGameFlow, Log, TEXT("[%s] Start game flow: %s."), *this->GetName(), *gameflows[curFlowIdx]->GetFlowName());
		gameflows[curFlowIdx]->OnEndFlow.BindUObject(this, &AELGameFlowManagerBase::OnGameFlowEnd);
		gameflows[curFlowIdx]->StartFlow();
	}
	else
	{
		UE_LOG(LogELGameFlow, Log, TEXT("[%s] reached end flow."), *this->GetName());
	}
}

void AELGameFlowManagerBase::DoFastForward(AELGameFlowBase *startFlow)
{
	AELGameFlowBase* curFlow = startFlow;
	curFlowIdx = gameflows.Find(startFlow);

	FString specifyNextFlow;
	while (curFlow != fastForwardTo)
	{
		// do fast forward
		specifyNextFlow.Empty();
		curFlow->FastForward(specifyNextFlow);

		// check next flow
		if (specifyNextFlow.IsEmpty())
		{
			++curFlowIdx;
			curFlow = gameflows[curFlowIdx];
		}
		else
		{
			curFlow = gameflowsMap[specifyNextFlow];
			if (curFlow == nullptr)
			{
				++curFlowIdx;
				curFlow = gameflows[curFlowIdx];
			}
			else
			{
				curFlowIdx = gameflows.Find(curFlow);
			}
		}
	}

	UE_LOG(LogELGameFlow, Log, TEXT("[%s] Start game flow: %s."), *this->GetName(), *curFlow->GetFlowName());
	curFlow->OnEndFlow.BindUObject(this, &AELGameFlowManagerBase::OnGameFlowEnd);
	curFlow->StartFlow();
}


