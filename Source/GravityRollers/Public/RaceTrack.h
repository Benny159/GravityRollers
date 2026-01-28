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
 */
UCLASS()
class GRAVITYROLLERS_API ARaceTrack : public AActor
{
	GENERATED_BODY()
	
public:    
	ARaceTrack();

	UFUNCTION(BlueprintCallable, Category = "Race Logic")
	void StartRace();
	
	UFUNCTION(BlueprintCallable, Category = "Race Logic")
	void SpawnMarble(TSubclassOf<AMarble> MarbleClass, int32 LaneIndex, const FMarbleData& InMarbleData);
	
	UFUNCTION(BlueprintCallable, Category = "Race Logic")
	void ResetTrack();

	UFUNCTION(BlueprintCallable, Category = "Race Control")
	void SetupRaceFromData(const TArray<FMarbleData>& MarblesData);

protected:
	virtual void BeginPlay() override;

	/** Creates the arrow components for start positions. Called in Constructor. */
	void CreateStartPositions();

	/** Caches references to GameMode and PlayerController to avoid repeated casting. */
	void CacheGameReferences();

	UFUNCTION()
	void InitialSpawnFromWorkbench();

	UFUNCTION()
	void OnEliminationZoneOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> TrackMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StartButton;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> EndTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> EliminationZone;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Start System")
	TObjectPtr<USceneComponent> StartPointsRoot;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Start System")
	TArray<TObjectPtr<UArrowComponent>> StartPositions;

	UPROPERTY(EditAnywhere, Category = "Start System")
	TSubclassOf<AMarble> RaceMarbleClass;

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<AMarble>> ActiveMarbles;

	/** Cached reference to the custom Game Mode. */
	UPROPERTY(Transient)
	TObjectPtr<AMarbleGameMode> MarbleGameMode;

	/** Cached reference to the custom Player Controller. */
	UPROPERTY(Transient)
	TObjectPtr<AMarblePlayerController> MarblePlayerController;

	bool bRaceStarted;
};