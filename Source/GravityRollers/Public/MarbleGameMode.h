// Copyright (c) 2026 Gravity Rollers. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MarbleGameMode.generated.h"

// Forward declarations to minimize header dependencies
class AMarble;

/**
 * Delegate used to broadcast when the race has officially ended.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRaceEndedSignature);

/**
 * AMarbleGameMode
 * Manages the rules, state flow, marble registration, and win conditions of the race.
 */
UCLASS()
class GRAVITYROLLERS_API AMarbleGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
    /** Set Default Values */
	AMarbleGameMode();

	/**
	 * Called every frame to update the race timer.
	 * @param DeltaSeconds Time elapsed since the last frame.
	 */
	virtual void Tick(float DeltaSeconds) override;

	/**
	 * Initiates the race sequence and resets previous data.
	 * @param NumberOfMarbles The expected number of marbles to finish/eliminate.
	 */
	UFUNCTION(BlueprintCallable, Category = "GravityRollers|Race Control")
	void StartRace(int32 NumberOfMarbles = 5);

	/**
	 * Registers a marble to the game mode for tracking.
	 * @param NewMarble The marble actor to register.
	 */
	UFUNCTION(BlueprintCallable, Category = "GravityRollers|Race Setup")
	void RegisterMarble(AMarble* NewMarble);

	/**
	 * Returns a list of marbles sorted by their final rank.
	 * @return Array of marbles.
	 */
	UFUNCTION(BlueprintCallable, Category = "GravityRollers|Race Info")
	TArray<AMarble*> GetMarblesSortedByRank();

	/**
	 * Resets the internal state, timers, and marble lists.
	 */
	UFUNCTION(BlueprintCallable, Category = "GravityRollers|Race Control")
	void ResetRaceState();

	/**
	 * Logic to handle a marble crossing the finish line.
	 */
	UFUNCTION(BlueprintCallable, Category = "GravityRollers|Race Logic")
	void RegisterMarbleFinished();

	/**
	 * Logic to handle a marble falling off the track.
	 */
	UFUNCTION(BlueprintCallable, Category = "GravityRollers|Race Logic")
	void RegisterMarbleEliminated();

	/**
	 * Returns the current duration of the race.
	 * @return Time in seconds.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GravityRollers|Race Info")
	float GetCurrentRaceTime() const;

	/**
	 * Forces the race to stop and cleans up remaining marbles.
	 */
	UFUNCTION(BlueprintCallable, Category = "GravityRollers|Game Flow")
	void ForceEndRace();

	/**
	 * Event triggered when the race logic determines the race is over.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "GravityRollers|Race Events")
	void RaceEnded();

protected:
	/**
	 * Checks if all marbles have finished or been eliminated.
	 * Triggers end of race logic if conditions are met.
	 */
	void CheckRaceStatus();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GravityRollers|Game Settings")
	bool bAllowWind;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GravityRollers|Game Settings")
	bool bAllowSeismic;

	UPROPERTY(BlueprintAssignable, Category = "GravityRollers|Race Events")
	FOnRaceEndedSignature OnRaceEnded;

	UPROPERTY(BlueprintReadOnly, Category = "GravityRollers|Race Data")
	TArray<AMarble*> RacingMarbles;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GravityRollers|Race State")
	bool bRaceActive;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GravityRollers|Race State")
	float RaceStartTime;

	UPROPERTY(BlueprintReadOnly, Category = "GravityRollers|Race State")
	float CurrentRaceTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GravityRollers|Race State")
	float RaceEndTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GravityRollers|Race State")
	int32 TotalMarbles;

private:
	int32 FinishedCount;
	int32 EliminatedCount;
};
