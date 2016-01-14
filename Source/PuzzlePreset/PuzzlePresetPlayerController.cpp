// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "PuzzlePreset.h"
#include "PuzzlePresetBlock.h"
#include "PuzzlePresetPlayerController.h"
#include "PuzzlePresetGameInstance.h"

#include "Online.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerState.h"

APuzzlePresetPlayerController::APuzzlePresetPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void APuzzlePresetPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerState)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, FString::Printf(TEXT("APuzzlePresetPlayerController created; player id: %d"), PlayerState->PlayerId));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, TEXT("APuzzlePresetPlayerController created; player state is null :("));
	}
}

void APuzzlePresetPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
}

void APuzzlePresetPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("TakeStone", IE_Pressed, this, &APuzzlePresetPlayerController::OnTakeStonePressed);
	InputComponent->BindAction("TakeStone", IE_Released, this, &APuzzlePresetPlayerController::OnTakeStoneReleased);

	InputComponent->BindAction("EndTurn", IE_Released, this, &APuzzlePresetPlayerController::OnEndTurn);
}

void APuzzlePresetPlayerController::OnTakeStonePressed()
{
	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);
	movedStone = Cast<APuzzlePresetBlock>(Hit.GetActor());
	if (Hit.bBlockingHit && movedStone != nullptr)
	{
		movedStone->SetIsDragged(true);
	}
}

void APuzzlePresetPlayerController::OnTakeStoneReleased()
{
	if (movedStone != nullptr)
	{
		if (movedStone->GetIsDragged())
		{
			movedStone->SetIsDragged(false);
			FVector newLocation = movedStone->GetBlockMesh()->GetComponentTransform().GetLocation();
			movedStone->SetActorLocation(newLocation, true);
			movedStone->GetBlockMesh()->SetWorldLocation(newLocation);
		}
	}
}

void APuzzlePresetPlayerController::OnEndTurn()
{
	/*UPuzzlePresetGameInstance* ppgi = Cast<UPuzzlePresetGameInstance>(GetGameInstance());
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	ppgi->HostSession(MakeShareable(LocalPlayer->GetControllerId()),)
	return;*/

	IOnlineSubsystem* ion = IOnlineSubsystem::Get(FName("Steam"));

	if (ion != nullptr)
	{
		ULocalPlayer* LocalPlayer = GetLocalPlayer();
		if (LocalPlayer && LocalPlayer->GetControllerId() != -1)
		{
			UE_LOG(LogTemp, Warning, TEXT("steam user name: %s"), *PlayerState->PlayerName);
			uint8 status = ion->GetIdentityInterface()->GetLoginStatus(LocalPlayer->GetControllerId());
			UE_LOG(LogTemp, Warning, TEXT("steam user status: %d"), status);
		}

		/*if (ion->InitSteamworksClient(false, 480))
		{
		UE_LOG(LogTemp, Warning, TEXT("Steam API initialized succesfully!"));
		}
		else
		{
		UE_LOG(LogTemp, Warning, TEXT("Failed to initialize Steam API :("));
		}*/
	}
}
