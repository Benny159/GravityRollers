#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Checkpoint.h"
#include "Marble.h"
#include "RaceTrack.generated.h"

UCLASS()
class GRAVITYROLLERS_API ARaceTrack : public AActor
{
    GENERATED_BODY()
    
public:    
    ARaceTrack();

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Track")
    UStaticMeshComponent* TrackMesh;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Track")
    UStaticMeshComponent* StartBarrier;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Track")
    UBoxComponent* EndTrigger;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Start System")
    USceneComponent* StartPointsRoot;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Start System")
    TArray<UArrowComponent*> StartPositions;

    UPROPERTY(EditAnywhere, Category = "Start System")
    TSubclassOf<AMarble> RaceMarbleClass;
    
    UFUNCTION(BlueprintCallable, Category = "Race Logic")
    void StartRace();
    
    UFUNCTION(BlueprintCallable, Category = "Race Logic")
    void SpawnMarble(TSubclassOf<AMarble> MarbleClass, int32 LaneIndex, const FMarbleData& MarbleData);
    
    UFUNCTION(BlueprintCallable, Category = "Race Logic")
    void ResetTrack();

    UFUNCTION(BlueprintCallable, Category = "Race Control")
    void SetupRaceFromData(const TArray<FMarbleData>& MarblesData);

private:
    UPROPERTY()
    TArray<ACheckpoint*> Checkpoints;
    
    UPROPERTY()
    TArray<AMarble*> ActiveMarbles;

    bool bRaceStarted;
    
    void RegisterCheckpoints();

    UFUNCTION()
    void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
