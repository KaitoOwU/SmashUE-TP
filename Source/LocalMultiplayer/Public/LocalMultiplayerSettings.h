// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LocalMultiplayerInputMappingType.h"
#include "LocalMultiplayerProfileData.h"
#include "Engine/DeveloperSettings.h"
#include "LocalMultiplayerSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Game, DefaultConfig, meta = (DisplayName="Local Multiplayer Settings"))
class LOCALMULTIPLAYER_API ULocalMultiplayerSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Local Multiplayer")
	TArray<FLocalMultiplayerProfileData> KeyboardProfilesData;

	UPROPERTY(Config, EditAnywhere, Category = "Local Multiplayer")
	FLocalMultiplayerProfileData GamepadProfileData;

	UPROPERTY(Config, EditAnywhere, Category = "Local Multiplayer")
	int NbMaxGamepads = 4;

	int GetNbKeyboardProfiles() const;
	int FindKeyboardProfileIndexFromKey(const FKey& Key, ELocalMultiplayerInputMappingType MappingType) const;
};
