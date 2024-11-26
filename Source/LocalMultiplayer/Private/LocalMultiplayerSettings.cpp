// Fill out your copyright notice in the Description page of Project Settings.


#include "LocalMultiplayerSettings.h"

int ULocalMultiplayerSettings::GetNbKeyboardProfiles() const
{
	return KeyboardProfilesData.Num();
}

int ULocalMultiplayerSettings::FindKeyboardProfileIndexFromKey(const FKey& Key,
	ELocalMultiplayerInputMappingType MappingType) const
{
	for (int i = 0; i < KeyboardProfilesData.Num(); ++i)
	{
		const FLocalMultiplayerProfileData& ProfileData = KeyboardProfilesData[i];
		if (ProfileData.GetIMCFromType(MappingType))
		{
			if(ProfileData.ContainsKey(Key, MappingType))
			{
				return i;
			}
		}
	}
	return -1;
}
