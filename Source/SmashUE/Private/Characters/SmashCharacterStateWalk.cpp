// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/SmashCharacterStateWalk.h"
#include "Characters/SmashCharacter.h"
#include "Characters/SmashCharacterStateID.h"
#include "Characters/SmashCharacterStateMachine.h"
#include "GameFramework/CharacterMovementComponent.h"

ESmashCharacterStateID USmashCharacterStateWalk::GetStateID()
{
	return ESmashCharacterStateID::Walk;
}

void USmashCharacterStateWalk::StateEnter(ESmashCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);

	Character->InputMoveXFastEvent.AddDynamic(this, &USmashCharacterStateWalk::OnInputMoveXFast);
}

void USmashCharacterStateWalk::StateExit(ESmashCharacterStateID NextStateID)
{
	Super::StateEnter(NextStateID);

	Character->GetCharacterMovement()->MaxWalkSpeed = MoveSpeedMax;
	
	Character->InputMoveXFastEvent.RemoveDynamic(this, &USmashCharacterStateWalk::OnInputMoveXFast);
}

void USmashCharacterStateWalk::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);
	
	if(FMathf::Abs(Character->GetInputMoveX()) < GetDefault<USmashCharacterSettings>()->InputMoveXThreshold)
	{
		StateMachine->ChangeState(ESmashCharacterStateID::Idle);
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

void USmashCharacterStateWalk::OnInputMoveXFast(float InputMoveX)
{
	StateMachine->ChangeState(ESmashCharacterStateID::Run);
}
