﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "Camera/CameraWorldSubsystem.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraFollowTarget.h"
#include "Kismet/GameplayStatics.h"

void UCameraWorldSubsystem::PostInitialize()
{
	Super::PostInitialize();
}

void UCameraWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	CameraMain = FindCameraByTag(TEXT("CameraMain"));
}

void UCameraWorldSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TickUpdateCameraPosition(DeltaTime);
}

void UCameraWorldSubsystem::AddFollowTarget(UObject* FollowTarget)
{
	FollowTargets.Add(FollowTarget);
}

void UCameraWorldSubsystem::RemoveFollowTarget(UObject* FollowTarget)
{
	FollowTargets.Remove(FollowTarget);
}

void UCameraWorldSubsystem::TickUpdateCameraPosition(float DeltaTime)
{
	if(CameraMain == nullptr) return;
	FVector AveragePosition = CalculateAveragePositionBetweenTargets();
	CameraMain->SetWorldLocation(AveragePosition);
}

FVector UCameraWorldSubsystem::CalculateAveragePositionBetweenTargets()
{
	float KeepY = CameraMain->GetComponentLocation().Y;
	FVector AveragePosition = FVector::ZeroVector;
	for(UObject* FollowTarget : FollowTargets) if (FollowTarget->Implements<UCameraFollowTarget>())
	{
		AveragePosition += Cast<ICameraFollowTarget>(FollowTarget)->GetFollowPosition();
	}
	AveragePosition /= FollowTargets.Num();
	return FVector(AveragePosition.X, KeepY, AveragePosition.Z);
}

UCameraComponent* UCameraWorldSubsystem::FindCameraByTag(const FName& Tag) const
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActors);
	for(AActor* Actor : FoundActors)
	{
		UCameraComponent* Camera = Actor->FindComponentByClass<UCameraComponent>();
		if(Camera != nullptr && Actor->ActorHasTag(Tag))
		{
			return Camera;
		}
	}
	return nullptr;
}






