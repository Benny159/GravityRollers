// Copyright (c) 2026 Gravity Rollers. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Marble.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStaticMeshComponent;

/**
 * Structure containing all persistent data for a marble instance.
 */
USTRUCT(BlueprintType)
struct FMarbleData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marble Data")
	FString MarbleName = TEXT("Marble");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marble Data")
	float Size = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marble Data")
	float Weight = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marble Data")
	float SurfaceRoughness = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marble Data")
	float MaterialDensity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marble Data")
	FVector MassDistribution = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marble Data")
	float Friction = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marble Data")
	float Restitution = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marble Data")
	float AngularDamping = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marble Data")
	int32 PreferredLaneIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marble Data")
	int32 FinalRank = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marble Data")
	float FinalRaceTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marble Data")
	float FinalRaceSpeed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marble Data")
	FLinearColor MarbleColor = FLinearColor::White;
};

/**
 * AMarble represents the physical marble controlled in the game.
 * It handles physics updates, selection logic, and race performance tracking.
 */
UCLASS()
class GRAVITYROLLERS_API AMarble : public AActor
{
	GENERATED_BODY()

public:
	AMarble();

	/**
	 * Called every frame to update camera and selection logic.
	 * @param DeltaTime Time elapsed since the last frame.
	 */
	virtual void Tick(float DeltaTime) override;

	/**
	 * Handles clicking interaction for selection.
	 * @param ButtonPressed The key used for clicking.
	 */
	virtual void NotifyActorOnClicked(FKey ButtonPressed = EKeys::LeftMouseButton) override;

	/**
	 * Sets the selection state and updates initial location for visuals.
	 * @param bSelected Whether the marble is currently selected.
	 */
	void SetSelected(bool bInSelected);

	/**
	 * Updates color and dynamic material parameters.
	 * @param NewColor The new color to apply to the marble mesh.
	 */
	UFUNCTION(BlueprintCallable, Category = "Marble|Appearance")
	void SetMarbleColor(FLinearColor NewColor);

	/**
	 * Sets up the marble based on external data.
	 * @param Data The struct containing all configuration values.
	 */
	UFUNCTION(BlueprintCallable, Category = "Marble|Configuration")
	void InitializeFromData(const FMarbleData& Data);

	/**
	 * Freezes or unfreezes physics simulation.
	 * @param bFrozen True to disable physics, false to enable.
	 */
	UFUNCTION(BlueprintCallable, Category = "Marble|Race Logic")
	void SetFrozen(bool bInFrozen);

	/**
	 * Records race finish stats and applies visual braking.
	 * @param TimeStamp The total race time at finish.
	 * @param FinishSpeed The velocity at the moment of crossing the line.
	 */
	void FinishRace(float TimeStamp, float FinishSpeed);

	/**
	 * Handles marble elimination upon crash or out-of-bounds.
	 */
	UFUNCTION(BlueprintCallable, Category = "Marble|Race Logic")
	void Eliminate();

	/**
	 * Packs current variables into an FMarbleData struct.
	 * @return A struct containing the current state of the marble.
	 */
	UFUNCTION(BlueprintCallable, Category = "Marble|Data")
	FMarbleData GetMarbleData() const;

	/** Returns the static mesh component. */
	FORCEINLINE UStaticMeshComponent* GetMesh() const { return MarbleMesh; }
	
protected:
	virtual void BeginPlay() override;

	/**
	 * Calculates and logs the current speed and race time to the DataTracker.
	 * Called every Tick during an active race.
	 */
	void UpdateRaceStats();

	/** Updates the physical material and mesh scale. */
	void UpdatePhysicsProperties();

	/** Creates a runtime physical material based on current attributes. */
	UPhysicalMaterial* CreatePhysicsMaterial();

	/** Smoothly interpolates selection visuals. */
	void UpdateSelectionVisuals(float DeltaTime);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Marble|Race Stats")
	bool bHasFinished;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Marble|Race Stats")
	bool bIsEliminated;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Marble|Race Stats")
	int32 StartingLaneIndex;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marble|Settings")
	FString MarbleName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MarbleMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FollowCamera;

	UPROPERTY(BlueprintReadOnly, Category = "Marble|Race Stats")
	int32 FinalRank;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* CameraBoom;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Marble|Physics")
	float ScaleFactor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Marble|Race Stats")
	float FinalRaceTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Marble|Race Stats")
	float FinalRaceSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marble|Physics")
	float Size;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marble|Physics")
	float Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marble|Physics")
	float SurfaceRoughness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marble|Physics")
	float MaterialDensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marble|Physics")
	FVector MassDistribution;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Marble|Physics")
	float Mass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marble|Physics")
	float Friction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marble|Physics")
	float Restitution;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marble|Physics")
	float AngularDamping;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Marble|Appearance")
	FLinearColor MarbleColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marble|Physics")
	TEnumAsByte<EFrictionCombineMode::Type> FrictionCombineMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marble|Physics")
	TEnumAsByte<EFrictionCombineMode::Type> RestitutionCombineMode;

private:
	bool bIsSelected;
	FVector InitialLocation;
};