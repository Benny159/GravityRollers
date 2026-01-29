// Copyright (c) 2026 Gravity Rollers. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Dog.generated.h"

// Forward declarations to reduce header dependencies
class AMarble;
class USkeletalMeshComponent;
class UAudioComponent;
class UCameraShakeBase;

/**
 * States representing the dog's current behavior.
 */
UENUM(BlueprintType)
enum class EDogState : uint8
{
	Sleeping    UMETA(DisplayName = "Sleeping"),
	Idle        UMETA(DisplayName = "Idle"),
	Jumping     UMETA(DisplayName = "Jumping")
};

/**
 * ADog
 * Represents an environmental hazard that causes seismic events (camera shake and physics impulses)
 * at random exponential intervals during the race.
 */
UCLASS()
class GRAVITYROLLERS_API ADog : public AActor
{
	GENERATED_BODY()

public:
	/** Creates Components. */
	ADog();
	
	/**
	 * Activates the internal timer loop to trigger seismic shocks.
	 */
	UFUNCTION(BlueprintCallable, Category = "GravityRollers|Race Logic")
	void StartShockLoop();

	/**
	 * Stops all timers and resets the dog to the appropriate state.
	 */
	UFUNCTION(BlueprintCallable, Category = "GravityRollers|Race Logic")
	void StopShockLoop();

	/**
	 * Toggles the dog's active state.
	 * @param bEnabled If true, the dog wakes up and can trigger shocks.
	 */
	UFUNCTION(BlueprintCallable, Category = "GravityRollers|Menu Logic")
	void SetDogEnabled(bool bEnabled);

protected:
	/** Set dogs State for animation. */
	virtual void BeginPlay() override;

private:
	/** Applies camera shake and physics impulses to marbles. */
	void ApplyShockEffects();

	/** Calculates a random delay and sets the timer for the next shock. */
	void ScheduleNextShock();

	/** Triggers the shock animation/sound and schedules the actual effect. */
	void PerformShock();

	/** Resets the dog state to Idle after a shock. */
	void ResetToIdle();

	/**
	 * Calculates a random float based on an exponential distribution.
	 * @param Mean The mean time between events.
	 * @return A random time interval.
	 */
	float GetExponentialRandom(float Mean);

	/** Refreshes the list of active racing marbles. */
	void UpdateMarbleCache();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GravityRollers|Seismic Settings")
	bool bIsDogEnabled;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GravityRollers|Seismic Settings")
	float MeanTimeBetweenShocks;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GravityRollers|Seismic Settings")
	float ShockStrength;
	
	UPROPERTY(EditAnywhere, Category = "GravityRollers|Seismic Settings")
	TSubclassOf<UCameraShakeBase> SeismicShakeClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GravityRollers|Animation")
	EDogState CurrentState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* DogMesh;

private:
	FTimerHandle TimerHandle_NextShock;
	FTimerHandle TimerHandle_ResetAnim;
	FTimerHandle TimerHandle_ShockDelay;

	UPROPERTY(Transient)
	TArray<AMarble*> CachedMarbles;
};