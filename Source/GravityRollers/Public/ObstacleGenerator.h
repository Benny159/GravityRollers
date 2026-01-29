// Copyright (c) 2026 Gravity Rollers. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Obstacle.h"
#include "ObstacleGenerator.generated.h"

/**
 * AObstacleGenerator
 * Manages the procedural generation of obstacles within a defined box area.
 * Handles placement, collision checking, and randomization of obstacle types.
 */
UCLASS()
class GRAVITYROLLERS_API AObstacleGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	/** Sets default values for this actor's properties. */
	AObstacleGenerator();
	
	/**
	 * Spawns a specified number of obstacles within the SpawnArea.
	 * Clears existing obstacles before generation.
	 */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "GravityRollers|Generation")
	void GenerateObstacles();

	/**
	 * Destroys all obstacles currently tracked by this generator.
	 */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "GravityRollers|Generation")
	void ClearObstacles();

protected:
	/** Called when the game starts or when spawned. */
	virtual void BeginPlay() override;

private:
	/**
	 * Tries to find a valid location and spawn a single obstacle.
	 * Repeats logic loop until successful or max attempts reached.
	 *
	 * @param TraceParams Parameters used for the ground detection raycast.
	 */
	void AttemptSpawnSingleObstacle(FCollisionQueryParams& TraceParams);

	/**
	 * Calculates a random start point and performs a raycast to find the floor.
	 *
	 * @param TraceParams Parameters for the line trace.
	 * @return The result of the raycast (Location, Normal, Actor, etc.).
	 */
	FHitResult GenerateRandomHitResult(FCollisionQueryParams& TraceParams);

	/**
	 * Helper to execute the actual LineTraceSingleByChannel.
	 *
	 * @param HitResult Reference to store the trace result.
	 * @param TraceStart Start point of the ray.
	 * @param TraceEnd End point of the ray.
	 * @param TraceParams Collision parameters to ignore specific actors.
	 */
	void SetHit(FHitResult& HitResult, FVector TraceStart, FVector TraceEnd, FCollisionQueryParams& TraceParams);

	/**
	 * Checks if the potential spawn location is obstructed by other objects.
	 *
	 * @param HitResult The potential ground location to check.
	 * @return True if the location is blocked by another obstacle or actor.
	 */
	bool IsBlocked(FHitResult& HitResult);

	/**
	 * Instantiates the obstacle actor at the validated location.
	 *
	 * @param HitResult The valid hit result containing location and rotation data.
	 */
	void PlaceSingleObstacle(FHitResult& HitResult);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* SpawnArea;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GravityRollers|Generation")
	TArray<TSubclassOf<AObstacle>> ObstacleTypes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GravityRollers|Generation")
	int32 ObstacleCount;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GravityRollers|Generation")
	float MinDistance;

private:
	UPROPERTY()
	TArray<AActor*> SpawnedObstacles;
	
	bool bHit;
	
	bool bSpawned;
};