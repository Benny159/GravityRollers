#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h" // WICHTIG: Include für Arrows
#include "Marble.h"
#include "MarbleWorkbench.generated.h"

UCLASS()
class GRAVITYROLLERS_API AMarbleWorkbench : public AActor
{
    GENERATED_BODY()
    
public:    
    AMarbleWorkbench();

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Workbench")
    UStaticMeshComponent* BoardMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Workbench")
    USceneComponent* SlotsRoot;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Workbench")
    TArray<UArrowComponent*> SlotArrows;
    
    UPROPERTY(EditAnywhere, Category = "Workbench Config")
    TSubclassOf<AMarble> MarbleClass;
    
    UPROPERTY(BlueprintReadOnly, Category = "Workbench Runtime")
    TArray<AMarble*> ConfigMarbles;

    UFUNCTION(BlueprintCallable, Category = "Race Logic")
    TArray<FMarbleData> GetAllMarbleData();

    UFUNCTION(BlueprintCallable, Category = "Logic")
    void SwapLaneAssignments(AMarble* TargetMarble, int32 DesiredLaneIndex);
};