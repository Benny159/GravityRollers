// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h" // Optional: Für Nummern über dem Checkpoint
#include "Checkpoint.generated.h"

UCLASS()
class GRAVITYROLLERS_API ACheckpoint : public AActor
{
	GENERATED_BODY()
    
public:    
	ACheckpoint();

protected:
	virtual void BeginPlay() override;

public:
	// --- KOMPONENTEN ---

	// Die unsichtbare Kollisionsbox (Der Auslöser)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* TriggerBox;

	// Das visuelle Modell (z.B. Torbogen, Fähnchen)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* CheckpointMesh;

	// --- LOGIK ---

	// Welcher Checkpoint ist das? (0, 1, 2...)
	// Wird von der RaceTrack automatisch gesetzt, kann aber im Editor geprüft werden
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Race Logic")
	int32 CheckpointIndex;

	// Hilfsfunktion zum Setzen des Index (durch RaceTrack)
	void SetCheckpointIndex(int32 NewIndex);

	// Das Event, wenn etwas durchfliegt
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
