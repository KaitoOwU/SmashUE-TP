// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CameraSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Game, DefaultConfig, meta = (DisplayName = "Smash Camera Settings"))
class SMASHUE_API UCameraSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category="Tags")
	FString CameraMainTag;

	UPROPERTY(Config, EditAnywhere, Category="Tags")
	FString CameraDistanceMinTag;

	UPROPERTY(Config, EditAnywhere, Category="Tags")
	FString CameraDistanceMaxTag;

	UPROPERTY(Config, EditAnywhere, Category="Tags")
	FString CameraBoundsTag;

	UPROPERTY(Config, EditAnywhere, Category="Distances")
	float DistanceBetweenTargetsMin;

	UPROPERTY(Config, EditAnywhere, Category="Distances")
	float DistanceBetweenTargetsMax;

	UPROPERTY(Config, EditAnywhere, Category="Damping")
	float SizeDampingFactor;

	UPROPERTY(Config, EditAnywhere, Category="Damping")
	float PositionDampingFactor;
};
