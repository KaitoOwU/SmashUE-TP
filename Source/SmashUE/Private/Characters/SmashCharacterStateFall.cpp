// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SmashCharacterStateFall.h"

#include "Characters/SmashCharacter.h"
#include "Characters/SmashCharacterStateMachine.h"
#include "GameFramework/CharacterMovementComponent.h"

class USmashCharacterSettings;

ESmashCharacterStateID USmashCharacterStateFall::GetStateID()
{
	return ESmashCharacterStateID::Fall;
}

void USmashCharacterStateFall::StateEnter(ESmashCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);

	Character->GetCharacterMovement()->GravityScale = FallGravityScale;
}

void USmashCharacterStateFall::StateExit(ESmashCharacterStateID NextStateID)
{
	Super::StateEnter(NextStateID);
}

void USmashCharacterStateFall::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);

	if(FMathf::Abs(Character->GetInputMoveX()) > GetDefault<USmashCharacterSettings>()->InputMoveXThreshold)
	{
		float MoveX = Character->GetInputMoveX();
		Character->AddMovementInput(FVector::ForwardVector, MoveX);
	}
	
	if(Character->GetVelocity().Z >= 0.f)
	{
		StateMachine->ChangeState(ESmashCharacterStateID::Idle);
	}
}
