// Fill out your copyright notice in the Description page of Project Settings.


#include "LocalMultiplayerProfileData.h"

#include "InputMappingContext.h"

bool FLocalMultiplayerProfileData::ContainsKey(const FKey& Key, ELocalMultiplayerInputMappingType MappingType) const
{
	UInputMappingContext* IMC = GetIMCFromType(MappingType);
	if (IMC)
	{
		for (const auto& Mapping : IMC->GetMappings())
		{
			if (Mapping.Key == Key)
			{
				return true;
			}
		}
	}
	return false;
}

UInputMappingContext* FLocalMultiplayerProfileData::GetIMCFromType(ELocalMultiplayerInputMappingType MappingType) const
{
	switch(MappingType)
	{
		case ELocalMultiplayerInputMappingType::InGame:
			return IMCInGame.Get();
		case ELocalMultiplayerInputMappingType::Menu:
			return IMCMenu.Get();
		default:
			return nullptr;
	}
}
