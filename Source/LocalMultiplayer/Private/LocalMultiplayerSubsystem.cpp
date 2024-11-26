// Fill out your copyright notice in the Description page of Project Settings.


#include "LocalMultiplayerSubsystem.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "LocalMultiplayerSettings.h"
#include "Kismet/GameplayStatics.h"

void ULocalMultiplayerSubsystem::CreateAndInitPlayers(ELocalMultiplayerInputMappingType MappingType)
{
	int NbProfiles = GetDefault<ULocalMultiplayerSettings>()->GetNbKeyboardProfiles() + GetDefault<ULocalMultiplayerSettings>()->NbMaxGamepads;
	for(int i = 0; i < NbProfiles; ++i)
	{
		UGameplayStatics::CreatePlayer(this, i);
	}
}

int ULocalMultiplayerSubsystem::GetAssignedPlayerIndexFromKeyboardProfileIndex(int KeyboardProfileIndex)
{
	return PlayerIndexFromKeyboardProfileIndex.Find(KeyboardProfileIndex) ? PlayerIndexFromKeyboardProfileIndex.FindRef(KeyboardProfileIndex) : -1;
}

int ULocalMultiplayerSubsystem::AssignNewPlayerToKeyboardProfile(int KeyboardProfileIndex)
{
	int PlayerIndex = LastAssignedPlayerIndex++;
	PlayerIndexFromKeyboardProfileIndex.Add(KeyboardProfileIndex, PlayerIndex);
	return PlayerIndex;
}

void ULocalMultiplayerSubsystem::AssignKeyboardMapping(int PlayerIndex, int KeyboardProfileIndex,
	ELocalMultiplayerInputMappingType MappingType) const
{
	FLocalMultiplayerProfileData KeyboardProfileData = GetDefault<ULocalMultiplayerSettings>()->KeyboardProfilesData[KeyboardProfileIndex];
	UInputMappingContext* IMC = KeyboardProfileData.GetIMCFromType(MappingType);
	ULocalPlayer* LocalPlayer = GetGameInstance()->GetLocalPlayerByIndex(PlayerIndex);

	if(IMC && LocalPlayer)
	{
		UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if(InputSubsystem)
		{
			InputSubsystem->AddMappingContext(IMC, 1);
		}
	}
}

int ULocalMultiplayerSubsystem::GetAssignedPlayerIndexFromGamepadDeviceID(int DeviceID)
{
	return PlayerIndexFromGamepadProfileIndex.Find(DeviceID) ? PlayerIndexFromGamepadProfileIndex.FindRef(DeviceID) : -1;
}

int ULocalMultiplayerSubsystem::AssignNewPlayerToGamepadDeviceID(int DeviceID)
{
	int PlayerIndex = LastAssignedPlayerIndex++;
	PlayerIndexFromGamepadProfileIndex.Add(DeviceID, PlayerIndex);
	return PlayerIndex;
}

void ULocalMultiplayerSubsystem::AssignGamepadInputMapping(int PlayerIndex,
	ELocalMultiplayerInputMappingType MappingType) const
{
	FLocalMultiplayerProfileData GamepadProfileData = GetDefault<ULocalMultiplayerSettings>()->GamepadProfileData;
	UInputMappingContext* IMC = GamepadProfileData.GetIMCFromType(MappingType);
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), PlayerIndex);

	if(IMC && PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* InputSubsystem = PlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if(InputSubsystem)
		{
			InputSubsystem->AddMappingContext(IMC, 1);
		}
	}
}


