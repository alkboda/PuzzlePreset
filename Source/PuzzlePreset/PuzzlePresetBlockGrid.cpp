// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "PuzzlePreset.h"
#include "PuzzlePresetBlockGrid.h"
#include "PuzzlePresetBlock.h"
#include "Components/TextRenderComponent.h"

#define LOCTEXT_NAMESPACE "PuzzleBlockGrid"

APuzzlePresetBlockGrid::APuzzlePresetBlockGrid()
{
	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	// Create static mesh component
	ScoreText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("ScoreText0"));
	ScoreText->SetRelativeLocation(FVector(200.f,0.f,0.f));
	ScoreText->SetRelativeRotation(FRotator(90.f,0.f,0.f));
	ScoreText->SetText(FText::Format(LOCTEXT("ScoreFmt", "Score: {0}"), FText::AsNumber(0)));
	ScoreText->AttachTo(DummyRoot);

	// Set defaults
	Size = 3;
	BlockSpacing = 300.f;
}

void APuzzlePresetBlockGrid::BeginPlay()
{
	Super::BeginPlay();

	// Number of blocks
	const int32 NumBlocks = Size * Size;

	// Loop to spawn each block
	for(int32 BlockIndex=0; BlockIndex<NumBlocks; BlockIndex++)
	{
		//const float XOffset = (BlockIndex/Size) * BlockSpacing; // Divide by dimension
		//const float YOffset = (BlockIndex%Size) * BlockSpacing; // Modulo gives remainder

		//// Make postion vector, offset from Grid location
		//const FVector BlockLocation = FVector(XOffset, YOffset, 0.f) + GetActorLocation();

		/*begin temp*/
		const FVector BlockLocation1 = FVector(BlockIndex % 2 == 0 ? 0.f: -62.5f, BlockIndex % 2 == 0 ? -62.5f : 0.f, BlockIndex * 62.5f) + GetActorLocation();
		const FVector BlockLocation2 = FVector(BlockIndex % 2 == 0 ? 0.f: 62.5f, BlockIndex % 2 == 0 ? 62.5f : 0.f, BlockIndex * 62.5f) + GetActorLocation();

		APuzzlePresetBlock* NewBlock = GetWorld()->SpawnActor<APuzzlePresetBlock>(BlockLocation1, FRotator(0, BlockIndex % 2 == 0 ? 0 : 90.f, 0));
		NewBlock->BlueMaterial = BlueMaterial;
		NewBlock->OrangeMaterial = OrangeMaterial;
		NewBlock->PostInitProperties();
		// Tell the block about its owner
		if (NewBlock != NULL)
		{
			NewBlock->OwningGrid = this;
		}
		/*end temp*/

		// Spawn a block
		/*APuzzlePresetBlock* */NewBlock = GetWorld()->SpawnActor<APuzzlePresetBlock>(BlockLocation2, FRotator(0, BlockIndex % 2 == 0 ? 0 : 90.f, 0));
		NewBlock->BlueMaterial = BlueMaterial;
		NewBlock->OrangeMaterial = OrangeMaterial;
		NewBlock->PostInitProperties();

		// Tell the block about its owner
		if(NewBlock != NULL)
		{
			NewBlock->OwningGrid = this;
		}
	}
}


void APuzzlePresetBlockGrid::AddScore()
{
	// Increment score
	Score++;

	// Update text
	ScoreText->SetText(FText::Format(LOCTEXT("ScoreFmt", "Score: {0}"), FText::AsNumber(Score)));
}

#undef LOCTEXT_NAMESPACE
