// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "PuzzlePreset.h"
#include "PuzzlePresetGameMode.h"
#include "PuzzlePresetPlayerController.h"

#include "GameFramework/PlayerState.h"

APuzzlePresetGameMode::APuzzlePresetGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// no pawn by default
	DefaultPawnClass = NULL;
	// use our own player controller class
	PlayerControllerClass = APuzzlePresetPlayerController::StaticClass();
}

void APuzzlePresetGameMode::PreLogin(const FString& Options, const FString& Address, const TSharedPtr<const FUniqueNetId>& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
}

void APuzzlePresetGameMode::PostLogin(APlayerController* NewPlayer)
{
	if (GetNumPlayers() >= 2)
	{
		bStartPlayersAsSpectators = true;
		NewPlayer->StartSpectatingOnly();
	}

	Super::PostLogin(NewPlayer);

	UE_LOG(LogTemp, Warning, TEXT("Totally connected: %d"), GetNumPlayers());
	UE_LOG(LogTemp, Warning, TEXT("Connected PC is spectating: %d"), NewPlayer->PlayerState->bIsSpectator);
}
