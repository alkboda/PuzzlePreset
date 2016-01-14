// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "PuzzlePresetGameMode.generated.h"

/** GameMode class to specify pawn and playercontroller */
UCLASS(minimalapi)
class APuzzlePresetGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	APuzzlePresetGameMode(const FObjectInitializer& ObjectInitializer);

	/**
	* Accept or reject a player attempting to join the server.  Fails login if you set the ErrorMessage to a non-empty string.
	* PreLogin is called before Login.  Significant game time may pass before Login is called, especially if content is downloaded.
	*
	* @param	Options					The URL options (e.g. name/spectator) the player has passed
	* @param	Address					The network address of the player
	* @param	UniqueId				The unique id the player has passed to the server
	* @param	ErrorMessage			When set to a non-empty value, the player will be rejected using the error message set
	*/
	virtual void PreLogin(const FString& Options, const FString& Address, const TSharedPtr<const FUniqueNetId>& UniqueId, FString& ErrorMessage) override;

	/** Called after a successful login.  This is the first place it is safe to call replicated functions on the PlayerAController. */
	virtual void PostLogin(APlayerController* NewPlayer) override;
};



