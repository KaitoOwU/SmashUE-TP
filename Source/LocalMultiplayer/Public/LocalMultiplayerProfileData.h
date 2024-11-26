// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LocalMultiplayerInputMappingType.h"
#include "UObject/Object.h"
#include "LocalMultiplayerProfileData.generated.h"

/**
 * 
 */

class UInputMappingContext;

USTRUCT()
struct FLocalMultiplayerProfileData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FName ProfileName;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputMappingContext> IMCInGame;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputMappingContext> IMCMenu;

	bool ContainsKey(const FKey& Key, ELocalMultiplayerInputMappingType MappingType) const;
	UInputMappingContext* GetIMCFromType(ELocalMultiplayerInputMappingType MappingType) const;	
};
