// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/ELPawnBase.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/PlayerInput.h"

UPawnMovementComponent* AELPawnBase::GetMovementComponent() const
{
	if (enableDesktopDebugControl)
	{
		UPawnMovementComponent* moveComp = Cast<UPawnMovementComponent>(GetComponentByClass(UPawnMovementComponent::StaticClass()));
		if (IsValid(moveComp)) return moveComp;
	}

	return Super::GetMovementComponent();
}

void AELPawnBase::InitializeDebugELPawnInputBindings()
{
	//static bool bELDebugPawnBindingsAdded = false;
	if (!bELDebugPawnBindingsAdded)
	{
		bELDebugPawnBindingsAdded = true;

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveForward", EKeys::W, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveForward", EKeys::S, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveForward", EKeys::Up, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveForward", EKeys::Down, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveForward", EKeys::Gamepad_LeftY, 1.f));

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveRight", EKeys::A, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveRight", EKeys::D, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveRight", EKeys::Gamepad_LeftX, 1.f));

		// HACK: Android controller bindings in ini files seem to not work
		//  Direct overrides here some to work
#if !PLATFORM_ANDROID
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveUp", EKeys::Gamepad_LeftThumbstick, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveUp", EKeys::Gamepad_RightThumbstick, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveUp", EKeys::Gamepad_FaceButton_Bottom, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveUp", EKeys::LeftControl, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveUp", EKeys::SpaceBar, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveUp", EKeys::C, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveUp", EKeys::E, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveUp", EKeys::Q, -1.f));
#else
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveUp", EKeys::Gamepad_LeftTriggerAxis, -0.5f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveUp", EKeys::Gamepad_RightTriggerAxis, 0.5f));
#endif

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_TurnRate", EKeys::Gamepad_RightX, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_TurnRate", EKeys::Left, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_TurnRate", EKeys::Right, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_Turn", EKeys::MouseX, 1.f));

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_LookUpRate", EKeys::Gamepad_RightY, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_LookUp", EKeys::MouseY, -1.f));
	}
}

void AELPawnBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	if (enableDesktopDebugControl)
	{
		if (bAddDefaultMovementBindings)
		{
			InitializeDebugELPawnInputBindings();

			PlayerInputComponent->BindAxis("DefaultPawn_MoveForward", this, &AELPawnBase::MoveForward);
			PlayerInputComponent->BindAxis("DefaultPawn_MoveRight", this, &AELPawnBase::MoveRight);
			PlayerInputComponent->BindAxis("DefaultPawn_MoveUp", this, &AELPawnBase::MoveUp_World);
			PlayerInputComponent->BindAxis("DefaultPawn_Turn", this, &AELPawnBase::AddControllerYawInput);
			PlayerInputComponent->BindAxis("DefaultPawn_TurnRate", this, &AELPawnBase::TurnAtRate);
			PlayerInputComponent->BindAxis("DefaultPawn_LookUp", this, &AELPawnBase::AddControllerPitchInput);
			PlayerInputComponent->BindAxis("DefaultPawn_LookUpRate", this, &AELPawnBase::LookUpAtRate);
		}
	}
	else
	{
		Super::SetupPlayerInputComponent(PlayerInputComponent);
	}
}

// Sets default values
AELPawnBase::AELPawnBase()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//if (enableDesktopDebugControl)
	//{
	//	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("VRDesktopDebugMove"));
	//	MovementComponent->UpdatedComponent = RootComponent;
	//}

	// This is the default pawn class, we want to have it be able to move out of the box.
	bAddDefaultMovementBindings = true;

}

void AELPawnBase::MoveForward(float Val)
{
	if (Val != 0.f)
	{
		if (Controller)
		{
			FRotator const ControlSpaceRot = Controller->GetControlRotation();

			// transform to world space and add it
			AddMovementInput(FRotationMatrix(ControlSpaceRot).GetScaledAxis(EAxis::X), Val*DebugMovementSpeed);
		}
	}
}

void AELPawnBase::MoveRight(float Val)
{
	if (Val != 0.f)
	{
		if (Controller)
		{
			FRotator const ControlSpaceRot = Controller->GetControlRotation();

			// transform to world space and add it
			AddMovementInput(FRotationMatrix(ControlSpaceRot).GetScaledAxis(EAxis::Y), Val*DebugMovementSpeed);
		}
	}
}

void AELPawnBase::MoveUp_World(float Val)
{
	if (Val != 0.f)
	{
		AddMovementInput(FVector::UpVector, Val*DebugMovementSpeed);
	}
}

void AELPawnBase::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds() * CustomTimeDilation);
}

void AELPawnBase::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds() * CustomTimeDilation);
}

//void AELPawnBase::OnConstruction(const FTransform & Transform)
//{
//	FTransform transform = GetActorTransform();
//	FActorSpawnParameters SpawnInfo;
//	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
//
//	/*if (SpawnObjects.Num() > 0)
//	{
//		for (AActor* actor : SpawnObjects)
//		{
//			actor->Destroy();
//		}
//		SpawnObjects.Empty();
//	}
//
//	for (TSubclassOf<AActor> mClass : SpawnClass)
//	{
//		AActor* actor = GetWorld()->SpawnActor(mClass.Get(), &transform, SpawnInfo);
//		SpawnObjects.Add(actor);
//	}*/
//
//	Super::OnConstruction(Transform);
//
//}