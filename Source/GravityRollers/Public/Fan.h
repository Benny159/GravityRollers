// Copyright (c) 2026 Gravity Rollers. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Fan.generated.h"

// Forward declarations to reduce header dependencies
class AMarble;
class UArrowComponent;
class UStaticMeshComponent;

/**
 * AFan
 * An interactive actor that rotates and applies a directional wind force 
 * to active marbles using a Gaussian distribution for wind variance.
 */
UCLASS()
class GRAVITYROLLERS_API AFan : public AActor
{
	GENERATED_BODY()
	
public:
	/** Create components and set default values. */
	AFan();
	
	/**
	 * Called every frame to handle rotation and wind physics.
	 * @param DeltaTime Time elapsed since the last frame.
	 */
	virtual void Tick(float DeltaTime) override;

	/**
	 * Toggles the fan's active state (On/Off).
	 */
	UFUNCTION(BlueprintCallable, Category = "GravityRollers|Interaction")
	void ToggleFan();

private:
	/**
	 * Generates a random number following a Gaussian (Normal) distribution.
	 * Used to simulate natural wind gust variance.
	 *  @param Mean The average value.
	 * @param StdDev The standard deviation (spread) from the mean.
	 * @return A randomized float value.
	 */
	float GetGaussianRandom(float Mean, float StdDev) const;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* RotorMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* WindDirectionArrow;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GravityRollers|Wind Physics")
	bool bIsFanActive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GravityRollers|Wind Physics")
	float MeanWindStrength;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GravityRollers|Wind Physics")
	float WindVariance;

	UPROPERTY(EditAnywhere, Category = "GravityRollers|Visuals")
	float RotationSpeed;
	
	UPROPERTY(Transient)
	TArray<AMarble*> ActiveMarbles;
};