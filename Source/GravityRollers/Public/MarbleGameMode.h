#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MarbleGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRaceEndedSignature);

UCLASS()
class GRAVITYROLLERS_API AMarbleGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AMarbleGameMode();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Settings")
    bool bAllowWind = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Settings")
    bool bAllowSeismic = true;

    virtual void Tick(float DeltaSeconds) override;
    
    UFUNCTION(BlueprintCallable, Category = "Race Control")
    void StartRace(int32 NumberOfMarbles = 5);

    UPROPERTY(BlueprintReadOnly, Category = "Race Data")
    TArray<AMarble*> RacingMarbles;
    
    UFUNCTION(BlueprintCallable, Category = "Race Setup")
    void RegisterMarble(AMarble* NewMarble);
    
    UFUNCTION(BlueprintCallable, Category = "Race Info")
    TArray<AMarble*> GetMarblesSortedByRank();
    
    UFUNCTION(BlueprintCallable, Category = "Race Control")
    void ResetRaceState();
    
    UFUNCTION(BlueprintCallable, Category = "Race Logic")
    void RegisterMarbleFinished();
    
    UFUNCTION(BlueprintCallable, Category = "Race Logic")
    void RegisterMarbleEliminated();

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Race Info")
    float GetCurrentRaceTime() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Race Info")
    bool IsRaceActive() const { return bRaceActive; }
    
    UPROPERTY(BlueprintAssignable, Category = "Race Events")
    FOnRaceEndedSignature OnRaceEnded;

    UFUNCTION(BlueprintImplementableEvent, Category = "Race Events")
    void RaceEnded();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Race State")
    bool bRaceActive;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Race State")
    float RaceStartTime;

    UPROPERTY(BlueprintReadOnly, Category = "Race")
    float CurrentRaceTime = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Race State")
    float RaceEndTime;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Race State")
    int32 TotalMarbles = 5;
    
    int32 FinishedCount;
    int32 EliminatedCount;
    
    void CheckRaceStatus();
};
