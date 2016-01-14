// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/PlayerController.h"
#include "PuzzlePresetBlock.h"
#include "PuzzlePresetPlayerController.generated.h"

/** PlayerController class used to enable cursor */
UCLASS()
class APuzzlePresetPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	APuzzlePresetPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;

	void OnTakeStonePressed();
	void OnTakeStoneReleased();
	void OnEndTurn();

private:
	APuzzlePresetBlock* movedStone;
};


