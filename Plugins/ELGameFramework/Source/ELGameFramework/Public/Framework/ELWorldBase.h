// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ELGameFramework.h"
#include "ELWorldBase.generated.h"

UCLASS(Blueprintable, BlueprintType)
class ELGAMEFRAMEWORK_API AELWorldBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AELWorldBase();

	/** Set enable of the world and systems in the world. */
	UFUNCTION(Server, Reliable, BlueprintCallable, WithValidation, Category = "ELGameFramework | ELWrold")
	void SetEnable(bool value);

	/** Add actor to world. */
	UFUNCTION(Server, Reliable, BlueprintCallable, WithValidation, Category = "ELGameFramework | ELWrold")
	virtual void AddActor(AActor* actor);

	/** Remove actor from world. */
	UFUNCTION(Server, Reliable, BlueprintCallable, WithValidation, Category = "ELGameFramework | ELWrold")
	virtual void RemoveActor(AActor* actor);

	/** Call after add component to actor to join related systems. */
	UFUNCTION(Server, Reliable, BlueprintCallable, WithValidation, Category = "ELGameFramework | ELWrold")
	virtual void PostAddComponent(AActor* actor);

	/** Call after destroy component from actor to leave related systems. */
	UFUNCTION(Server, Reliable, BlueprintCallable, WithValidation, Category = "ELGameFramework | ELWrold")
	virtual void PostRemoveComponent(AActor* actor);

	/** Check actor is in world. */
	UFUNCTION(BlueprintCallable, Category = "ELGameFramework | ELWrold")
	bool ActorIsInWorld(AActor* actor);

	/** Add system to world. */
	UFUNCTION(Server, Reliable, BlueprintCallable, WithValidation, Category = "ELGameFramework | ELWrold")
	virtual void AddSystem(AELSystemBase* system);

	/** Get system by name. */
	UFUNCTION(BlueprintCallable, Category = "ELGameFramework | ELWrold")
	AELSystemBase* GetSystem(FString systemName);

	FString GetWorldName() { return name; };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "ELGameFramework")
	FString name;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "ELGameFramework")
	bool enable = true;

	UPROPERTY(Replicated, EditInstanceOnly, BlueprintReadOnly, Category = "ELGameFramework")
	TArray<AActor*> actors;

	UPROPERTY(Replicated, EditInstanceOnly, BlueprintReadOnly, Category = "ELGameFramework")
	TArray<AELSystemBase*> systems;

private:
	void ConstructSystemsMap();

	UFUNCTION()
	virtual void OnActorDestroyed(AActor * actor);

#if WITH_EDITOR
	void PrintWorldAndSystemInfo();
#endif
	UPROPERTY(Replicated)
	bool systemsMapConstructed = false;

	UPROPERTY(VisibleAnywhere, Category = "ELGameFramework")
	TMap<FString, AELSystemBase*> systemsMap;

	UPROPERTY(VisibleAnywhere, Category = "ELGameFramework")
	TArray<AELSystemBase*> redundantSystems;

};
