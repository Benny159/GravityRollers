// Copyright (c) 2026 Gravity Rollers. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarbleWorkbench.generated.h"

// Forward declarations to reduce compile time
class UArrowComponent;
class UStaticMeshComponent;
class USceneComponent;
class AMarble;
struct FMarbleData;

/**
 * AMarbleWorkbench manages the configuration area for marbles.
 * It handles spawning config marbles, assigning colors, and managing lane swaps.
 */
UCLASS()
class GRAVITYROLLERS_API AMarbleWorkbench : public AActor
{
	GENERATED_BODY()
	
public:    
	AMarbleWorkbench();

	/**
	 * Retrieves the data of all currently configured marbles.
	 * @return An array of FMarbleData structs representing the current state.
	 */
	UFUNCTION(BlueprintCallable, Category = "Workbench|Data")
	TArray<FMarbleData> GetAllMarbleData() const;

	/**
	 * Swaps the logical lane assignment between two marbles.
	 * Updates names and internal indices without moving the physical actors.
	 * @param TargetMarble The marble initiating the swap.
	 * @param DesiredLaneIndex The index of the lane to swap to (0-4).
	 */
	UFUNCTION(BlueprintCallable, Category = "Workbench|Logic")
	void SwapLaneAssignments(AMarble* TargetMarble, int32 DesiredLaneIndex);

protected:
	virtual void BeginPlay() override;

	/** Spawns the marbles at the arrow locations defined in the constructor. */
	void SpawnConfigMarbles();

private:
	/** Creates the arrow components for the slots. Internal use only. */
	void CreateSlotArrows();

public:
	UPROPERTY(EditAnywhere, Category = "Workbench|Config")
	TSubclassOf<AMarble> MarbleClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Workbench|Config")
	TArray<FLinearColor> DefaultMarbleColors;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Workbench|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> BoardMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Workbench|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SlotsRoot;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Workbench|Components", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UArrowComponent>> SlotArrows;
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Workbench|Runtime", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<AMarble>> ConfigMarbles;
};