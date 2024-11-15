// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/SmashCharacterStateRun.h"
#include "Characters/SmashCharacter.h"
#include "Characters/SmashCharacterStateID.h"
#include "Characters/SmashCharacterStateMachine.h"
#include "GameFramework/CharacterMovementComponent.h"

ESmashCharacterStateID USmashCharacterStateRun::GetStateID()
{
	return ESmashCharacterStateID::Run;
}

void USmashCharacterStateRun::StateEnter(ESmashCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);

	Character->GetCharacterMovement()->MaxWalkSpeed = RunSpeedMax;

	GEngine->AddOnScreenDebugMessage(
		-1,
		3.f,
		FColor::Cyan,
		TEXT("Enter State Run"));
}

void USmashCharacterStateRun::StateExit(ESmashCharacterStateID NextStateID)
{
	Super::StateEnter(NextStateID);

	GEngine->AddOnScreenDebugMessage(
		-1,
		3.f,
		FColor::Red,
		TEXT("Exit State Run"));
}

void USmashCharacterStateRun::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);

	if(FMathf::Abs(Character->GetInputMoveX()) < 0.8f)
	{
		StateMachine->ChangeState(ESmashCharacterStateID::Walk);
	} else
	{
		Character->SetOrientX(FMathf::Sign(Character->GetInputMoveX()) * 1);
		
		float MoveX = Character->GetInputMoveX();
		Character->AddMovementInput(FVector::ForwardVector, MoveX);
	}

	if(Character->GetInputJump() > GetDefault<USmashCharacterSettings>()->InputJumpThreshold)
	{
		StateMachine->ChangeState(ESmashCharacterStateID::Jump);
	}
}