// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "PuzzlePreset.h"
#include "PuzzlePresetBlock.h"
#include "PuzzlePresetBlockGrid.h"

APuzzlePresetBlock::APuzzlePresetBlock()
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		ConstructorHelpers::FObjectFinderOptional<UMaterial> BlueMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterial> OrangeMaterial;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Puzzle/Meshes/PuzzleCube.PuzzleCube"))
			, BlueMaterial(TEXT("/Game/StarterContent/Materials/M_Rock_Basalt.M_Rock_Basalt"))
			, OrangeMaterial(TEXT("/Game/StarterContent/Materials/M_Metal_Rust.M_Metal_Rust"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	PrimaryActorTick.bCanEverTick = true;

	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	// Setup static mesh
	//UStaticMesh* mesh = ConstructorStatics.PlaneMesh.Get();

	// Create static mesh component
	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
	BlockMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
	BlockMesh->SetRelativeScale3D(FVector(1.f,0.5f,0.25f));
	BlockMesh->SetRelativeLocation(FVector(0.f,0.f,31.25f));
	BlockMesh->SetMaterial(0, ConstructorStatics.BlueMaterial.Get());
	BlockMesh->AttachTo(DummyRoot);
	BlockMesh->OnClicked.AddDynamic(this, &APuzzlePresetBlock::BlockClicked);
	BlockMesh->OnInputTouchBegin.AddDynamic(this, &APuzzlePresetBlock::OnFingerPressedBlock);

	BlockMesh->SetMobility(EComponentMobility::Movable);
	BlockMesh->SetSimulatePhysics(true);
	BlockMesh->SetLinearDamping(0.001f);

	// Save a pointer to the orange material
	BlueMaterial = ConstructorStatics.BlueMaterial.Get();
	OrangeMaterial = ConstructorStatics.OrangeMaterial.Get();
}

void APuzzlePresetBlock::PostInitProperties()
{
	Super::PostInitProperties();

	BlockMesh->SetMaterial(0, BlueMaterial);
}

void APuzzlePresetBlock::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsDragged)
	{
		// Trace to see what is under the mouse cursor
		FHitResult Hit;
		GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECC_Visibility, false, Hit);

		if (Hit.bBlockingHit)
		{
			if (dragDiff == FVector::ZeroVector)
			{
				dragDiff = GetActorLocation();
				dragDiff -= Hit.ImpactPoint;
			}

			//Hit.ImpactPoint += dragDiff;
			Hit.ImpactPoint.Z = GetActorLocation().Z;

			// We hit something, move there
			SetActorLocation(Hit.ImpactPoint, true);
			//UpdateComponentTransforms();
			//UpdateOverlaps();
		}
	}
	else
	{
		dragDiff = FVector::ZeroVector;
	}
}

void APuzzlePresetBlock::BlockClicked(UPrimitiveComponent* ClickedComp)
{
	bIsActive = !bIsActive;

	// Change material
	BlockMesh->SetMaterial(0, bIsActive ? OrangeMaterial : BlueMaterial);

	// Tell the Grid
	if(OwningGrid != NULL)
	{
		OwningGrid->AddScore();
	}
}

void APuzzlePresetBlock::OnFingerPressedBlock(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent)
{
	BlockClicked(TouchedComponent);
}