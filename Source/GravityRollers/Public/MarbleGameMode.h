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
    
    UFUNCTION(BlueprintCallable, Category = "Race Control")
    void StartRace(int32 NumberOfMarbles = 5);
    
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

protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Race State")
    bool bRaceActive;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Race State")
    float RaceStartTime;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Race State")
    float RaceEndTime;
    
    int32 TotalMarbles;
    
    int32 FinishedCount;
    int32 EliminatedCount;
    
    void CheckRaceStatus();
};
