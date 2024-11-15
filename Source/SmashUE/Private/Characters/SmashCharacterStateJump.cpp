// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SmashCharacterStateJump.h"

#include "Characters/SmashCharacter.h"
#include "Characters/SmashCharacterStateMachine.h"
#include "GameFramework/CharacterMovementComponent.h"

class USmashCharacterSettings;

ESmashCharacterStateID USmashCharacterStateJump::GetStateID()
{
	return ESmashCharacterStateID::Jump;
}

void USmashCharacterStateJump::StateEnter(ESmashCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);

	UCharacterMovementComponent* Movement = Character->GetCharacterMovement();
	Movement->AirControl = JumpAirControl;
	Movement->GravityScale = FMathf::Abs(((-2.f * JumpMaxHeight) / (JumpDuration * JumpDuration))) / 981.f;
	Movement->JumpZVelocity = (2.f * JumpMaxHeight) / JumpDuration;
	
	Character->Jump();
}

void USmashCharacterStateJump::StateExit(ESmashCharacterStateID NextStateID)
{
	Super::StateEnter(NextStateID);
	
}

void USmashCharacterStateJump::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);

	if(Character->GetVelocity().Z < 0.f)
	{
		StateMachine->ChangeState(ESmashCharacterStateID::Fall);
	}

	if(FMathf::Abs(Character->GetInputMoveX()) > GetDefault<USmashCharacterSettings>()->InputMoveXThreshold)
	{
		float MoveX = Character->GetInputMoveX();
		Character->AddMovementInput(FVector::ForwardVector, MoveX);
	}
}

