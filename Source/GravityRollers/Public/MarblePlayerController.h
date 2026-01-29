// Copyright (c) 2026 Gravity Rollers. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "GameFramework/PlayerController.h"
#include "MarblePlayerController.generated.h"

class AMarble;

/**
 * AMarblePlayerController
 * Controls the player's interaction during the marble race, including camera management and marble selection.
 */
UCLASS()
class GRAVITYROLLERS_API AMarblePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	/** Set Default Values */
	AMarblePlayerController();

	/**
	 * Switches the view to the main race camera.
	 */
	UFUNCTION(BlueprintCallable, Category = "GravityRollers|Camera")
	void SwitchToRaceView();

	/**
	 * Switches the view to the configuration/workbench camera.
	 */
	UFUNCTION(BlueprintCallable, Category = "GravityRollers|Camera")
	void SwitchToConfigView();

	/**
	 * Transitions from the race view back to the config view using a ghost camera for blending.
	 * @param LastCameraLocation Location of the camera at the moment of switching.
	 * @param LastCameraRotation Rotation of the camera at the moment of switching.
	 */
	UFUNCTION(BlueprintCallable, Category = "GravityRollers|Camera")
	void SwitchToConfigViewFromRace(FVector LastCameraLocation, FRotator LastCameraRotation);

	/**
	 * Switches the view to the analysis camera.
	 */
	UFUNCTION(BlueprintCallable, Category = "GravityRollers|Camera")
	void SwitchToAnalysView();

	/**
	 * Cycles the camera focus to the next active (not eliminated/finished) marble.
	 */
	UFUNCTION(BlueprintCallable, Category = "GravityRollers|Camera")
	void SwitchToNextActiveMarble();

	/**
	 * Focuses the camera on a specific marble identified by its lane index.
	 * @param MarbleIndex The starting lane index of the marble.
	 */
	UFUNCTION(BlueprintCallable, Category = "GravityRollers|Camera")
	void FocusOnMarble(int32 MarbleIndex);

	/**
	 * Updates the internal state regarding whether the race is currently active.
	 * @param bInActive True if the race is active.
	 */
	UFUNCTION(BlueprintCallable, Category = "GravityRollers|Game State")
	void SetRaceState(bool bInActive);

	/**
	 * Returns the current race state.
	 * @return True if the race is active.
	 */
	UFUNCTION(BlueprintPure, Category = "GravityRollers|Game State")
	bool IsRaceActive() const;

	/**
	 * Selects a marble for the UI/Workbench logic.
	 * @param NewMarble The marble actor to select.
	 */
	UFUNCTION(BlueprintCallable, Category = "GravityRollers|Game State")
	void SelectMarble(AMarble* NewMarble);

	/**
	 * Event triggered to show the UI for a specific marble.
	 * @param Marble The marble to display information for.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "GravityRollers|UI")
	void OnShowMarbleUI(AMarble* Marble);

	/**
	 * Event triggered to hide the marble UI.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "GravityRollers|UI")
	void OnHideMarbleUI();

protected:
	/** Initialize the player view and clean up data sets */
	virtual void BeginPlay() override;

	/** Setup input component to switch views */
	virtual void SetupInputComponent() override;

private:
	/** Destroys the temporary ghost camera used for transitions. */
	UFUNCTION()
	void CleanupGhostCamera();

	/** Resets the camera switching lock, allowing new camera inputs. */
	UFUNCTION()
	void UnlockCameraSwitch();

	/**
	 * Helper function to find a camera actor by its tag.
	 * @param Tag The tag to search for.
	 * @return The found actor or nullptr.
	 */
	UFUNCTION()
	AActor* FindCameraByTag(FName Tag);

	/** Input callback for View 1 */
	void ViewMarble1();
	/** Input callback for View 2 */
	void ViewMarble2();
	/** Input callback for View 3 */
	void ViewMarble3();
	/** Input callback for View 4 */
	void ViewMarble4();
	/** Input callback for View 5 */
	void ViewMarble5();

public:
	UPROPERTY(BlueprintReadOnly, Category = "GravityRollers|State")
	AMarble* CurrentSelectedMarble;

	UPROPERTY(BlueprintReadOnly, Category = "GravityRollers|State")
	AMarble* CurrentViewedMarble;

private:
	UPROPERTY(Transient)
	ACameraActor* GhostCameraActor;

	UPROPERTY()
	FTimerHandle TimerHandle_CleanupGhost;

	UPROPERTY()
	FTimerHandle TimerHandle_CameraCooldown;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GravityRollers|Internal", meta = (AllowPrivateAccess = "true"))
	bool bRaceIsActive;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GravityRollers|Internal", meta = (AllowPrivateAccess = "true"))
	bool bIsCameraSwitching;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GravityRollers|Internal", meta = (AllowPrivateAccess = "true"))
	int32 CurrentViewIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GravityRollers|Camera", meta = (AllowPrivateAccess = "true"))
	float SwitchTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GravityRollers|Camera", meta = (AllowPrivateAccess = "true"))
	float VirtualBlendTime;
};
