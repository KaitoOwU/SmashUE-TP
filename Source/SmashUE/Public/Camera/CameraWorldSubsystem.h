﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CameraWorldSubsystem.generated.h"

class UCameraComponent;
/**
 * 
 */
UCLASS()
class SMASHUE_API UCameraWorldSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

#pragma region Subsystem Override
	
public:
	virtual void PostInitialize() override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override { return TStatId();}
	
#pragma endregion

#pragma region Follow Targets
	
public:
	void AddFollowTarget(UObject* FollowTarget);
	void RemoveFollowTarget(UObject* FollowTarget);
	
#pragma endregion

#pragma region Main Camera

protected:
	UPROPERTY()
	TObjectPtr<UCameraComponent> CameraMain;

#pragma endregion

#pragma region Bounds

protected:
	UPROPERTY()
	FVector2D CameraBoundsMin;

	UPROPERTY()
	FVector2D CameraBoundsMax;

	UPROPERTY()
	float CameraBoundsYProjectionCenter;

	AActor* FindCameraBoundsActor();
	void InitCameraBounds(AActor* CameraBoundsActor);
	void ClampPositionIntoCameraBounds(FVector& Position);
	void GetViewportBounds(FVector2D& OutViewportBoundsMin, FVector2D& OutViewportBoundsMax);
	FVector CalculateWorldPositionFromViewportPosition(const FVector2D& ViewportPosition);
	
#pragma endregion 

#pragma region Misc
	
protected:
	UPROPERTY()
	TArray<UObject*> FollowTargets;

	void TickUpdateCameraPosition(float DeltaTime);
	FVector CalculateAveragePositionBetweenTargets();
	UCameraComponent* FindCameraByTag(const FName& Tag) const;

#pragma endregion
	
};