// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Actor.h"
#include "PuzzlePresetBlock.generated.h"

/** A block that can be clicked */
UCLASS(minimalapi)
class APuzzlePresetBlock : public AActor
{
	GENERATED_BODY()

	/** Dummy root component */
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DummyRoot;

	/** StaticMesh component for the clickable block */
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* BlockMesh;

private:
	/** Are we currently active? */
	bool bIsActive;

	bool bIsDragged;

	FVector dragDiff;
public:
	APuzzlePresetBlock();

	/** Pointer to blue material used on deactive blocks */
	UPROPERTY()
	class UMaterial* BlueMaterial;
	
	/** Pointer to orange material used on active blocks */
	UPROPERTY()
	class UMaterial* OrangeMaterial;

	/** Grid that owns us */
	UPROPERTY()
	class APuzzlePresetBlockGrid* OwningGrid;

	/** Called on initialization*/
	virtual void PostInitProperties() override;
	/** Called every frame */
	virtual void Tick(float DeltaSeconds) override;

	/** Handle the block being clicked */
	UFUNCTION()
	void BlockClicked(UPrimitiveComponent* ClickedComp);

	/** Handle the block being touched  */
	UFUNCTION()
	void OnFingerPressedBlock(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent);

public:
	/** Returns DummyRoot subobject **/
	FORCEINLINE class USceneComponent* GetDummyRoot() const { return DummyRoot; }
	/** Returns BlockMesh subobject **/
	FORCEINLINE class UStaticMeshComponent* GetBlockMesh() const { return BlockMesh; }

	FORCEINLINE void APuzzlePresetBlock::SetIsDragged(bool IsDragged) { bIsDragged = IsDragged; }

	FORCEINLINE bool APuzzlePresetBlock::GetIsDragged() const { return bIsDragged; }
};



