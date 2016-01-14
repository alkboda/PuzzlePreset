// Fill out your copyright notice in the Description page of Project Settings.

#include "PuzzlePreset.h"
#include "PuzzleStoneCard.h"


// Sets default values
APuzzleStoneCard::APuzzleStoneCard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APuzzleStoneCard::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APuzzleStoneCard::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

