// Copyright (c) 2026 Gravity Rollers. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RaceTrack.generated.h"

// Forward Declarations
class UBoxComponent;
class UArrowComponent;
class UStaticMeshComponent;
class AMarble;
class AMarbleGameMode;
class AMarblePlayerController;
struct FMarbleData;

/**
 * ARaceTrack manages the physical track, spawning logic, and win/loss conditions.
 * It coordinates the start sequence, hazards (Fan, Dog), and the finish line logic.
 */
UCLASS()
class GRAVITYROLLERS_API ARaceTrack : public AActor
{
    GENERATED_BODY()
    
public:    
    /** Sets default values for this actor's properties and creates the trigger components. */
    ARaceTrack();

    /**
	 * Begins the race sequence.
     * Unfreezes all active marbles, switches player camera to race mode, 
     * and activates environmental hazards (Fan, Dog).
     */
    UFUNCTION(BlueprintCallable, Category = "Race Logic")
    void StartRace();
    
    /**
     * Spawns a single marble instance at a specific lane in a FROZEN state.
     * The marble will not simulate physics until StartRace() is called.
     * @param MarbleClass The class of the marble actor to spawn.
     * @param LaneIndex The index of the start position (0-4).
     * @param InMarbleData The configuration struct applied to the marble after spawn.
     */
    UFUNCTION(BlueprintCallable, Category = "Race Logic")
    void SpawnMarble(TSubclassOf<AMarble> MarbleClass, int32 LaneIndex, const FMarbleData& InMarbleData);
    
    /**
     * Resets the track and race state.
     * Destroys active marbles, handles the camera transition back to the config menu,
     * and respawns the initial marble set from the Workbench.
     */
    UFUNCTION(BlueprintCallable, Category = "Race Logic")
    void ResetTrack();

    /**
     * Clears existing marbles and spawns a fresh set based on the provided data array.
     * @param MarblesData An array of FMarbleData structs defining the new participants.
     */
    UFUNCTION(BlueprintCallable, Category = "Race Control")
    void SetupRaceFromData(const TArray<FMarbleData>& MarblesData);

protected:
    /**
	 * Called when the game starts. 
     * Caches references and sets a timer to trigger InitialSpawnFromWorkbench. 
     */
    virtual void BeginPlay() override;

    /**
	 * Creates the arrow components used as spawn points. 
     * Generates 5 positions with fixed offsets. Called in Constructor. 
     */
    void CreateStartPositions();

    /**
	 * Caches references to GameMode and PlayerController.
     * Logs an error if essential game framework classes are missing.
     */
    void CacheGameReferences();

    /** 
	 * Retrieves marble data from the AMarbleWorkbench actor.
     * Called automatically via timer in BeginPlay to ensure Workbench is initialized.
     */
    UFUNCTION()
    void InitialSpawnFromWorkbench();

    /**
     * Handles the overlap event when a marble falls into the elimination zone.
     * Eliminates the marble, sets rank to 99, and auto-switches the camera if needed.
     * @param OverlappedComp The component that triggered the overlap.
     * @param OtherActor The actor that hit the trigger.
     * @param OtherComp The specific component of the other actor.
     * @param OtherBodyIndex The body index of the other component.
     * @param bFromSweep Whether the overlap was caused by a sweep.
     * @param SweepResult The hit result of the sweep.
     */
    UFUNCTION()
    void OnEliminationZoneOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
    /**
     * Handles the overlap event when a marble crosses the finish line trigger.
     * Records finish time/speed, updates GameMode stats, and auto-switches the camera.
     * @param OverlappedComp The component that triggered the overlap.
     * @param OtherActor The actor that hit the trigger.
     * @param OtherComp The specific component of the other actor.
     * @param OtherBodyIndex The body index of the other component.
     * @param bFromSweep Whether the overlap was caused by a sweep.
     * @param SweepResult The hit result of the sweep.
     */
    UFUNCTION()
    void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> TrackMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> StartButton;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UBoxComponent> EndTrigger;
