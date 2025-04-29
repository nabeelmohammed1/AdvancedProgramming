// Copyright Epic Games, Inc. All Rights Reserved.

#include "AdvancedProgrammingGameMode.h"
#include "AdvancedProgrammingCharacter.h"
#include "UObject/ConstructorHelpers.h"

AAdvancedProgrammingGameMode::AAdvancedProgrammingGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
