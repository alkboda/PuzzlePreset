// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "PuzzlePresetPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEPRESET_API APuzzlePresetPlayerState : public APlayerState
{
	GENERATED_BODY()

private:
	bool bIsMakingTurn;

public:
	APuzzlePresetPlayerState();	
	
};
