// Fill out your copyright notice in the Description page of Project Settings.

#include "Scene/ARPlaneActorBase.h"

// Sets default values
AARPlaneActorBase::AARPlaneActorBase()
{
	bReplicates = true;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	RootComponent = mesh;
}

// Called when the game starts or when spawned
//void AARPlaneActorBase::BeginPlay()
//{
//	Super::BeginPlay();
//	
//}

// Called every frame
//void AARPlaneActorBase::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

void AARPlaneActorBase::UpdatePlaneMesh(const TArray<FVector>& Vertices, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FVector2D>& UV0, const TArray<FLinearColor>& VertexColors, const TArray<FProcMeshTangent>& Tangents, bool bCreateCollision)
{
	mesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV0, VertexColors, Tangents, bCreateCollision);
}
