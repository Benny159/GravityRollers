#include "MarbleGameMode.h"
#include "DataTrackerPlugin/DataTracker.h"
#include "Dog.h"
#include "Kismet/GameplayStatics.h"
#include "MarblePlayerController.h"

AMarbleGameMode::AMarbleGameMode()
{
    bRaceActive = false;
    RaceStartTime = 0.0f;
    RaceEndTime = 0.0f;
    
    TotalMarbles = 5;
    FinishedCount = 0;
    EliminatedCount = 0;
}

void AMarbleGameMode::RegisterMarble(AMarble* NewMarble)
{
    if (NewMarble && !RacingMarbles.Contains(NewMarble))
    {
        RacingMarbles.Add(NewMarble);
        UE_LOG(LogTemp, Log, TEXT("Murmel registriert: %s. Total: %d"), *NewMarble->GetName(), TotalMarbles);
    }
}

TArray<AMarble*> AMarbleGameMode::GetMarblesSortedByRank()
{
    // 1. Bereinigen (Nullpointer entfernen)
    RacingMarbles.RemoveAll([](AMarble* Marble) {
        return Marble == nullptr || !IsValid(Marble);
    });

    TArray<AMarble*> SortedList = RacingMarbles;
    
    // 2. Sortieren (Primär nach Rang)
    SortedList.Sort([](const AMarble& A, const AMarble& B)
    {
        // Falls Ränge gleich sind (z.B. beide 99 oder beide 0), sekundär sortieren?
        // Hier erstmal dein Original:
        return A.FinalRank < B.FinalRank;
    });

    // --- DEBUG LOGGING ANFANG ---
    UE_LOG(LogTemp, Warning, TEXT("============== MARBLE RANKING DEBUG =============="));
    for (int32 i = 0; i < SortedList.Num(); i++)
    {
        AMarble* M = SortedList[i];
        if (M)
        {
            // Wir loggen Name, Rang, Zeit und Status
            UE_LOG(LogTemp, Warning, TEXT("#%d | Name: %s | Rank: %d | Time: %.2f | Eliminiert: %s"), 
                i + 1, 
                *M->GetName(), 
                M->FinalRank, 
                M->FinalRaceTime, 
                M->bIsEliminated ? TEXT("JA") : TEXT("NEIN")
            );
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("=================================================="));
    // --- DEBUG LOGGING ENDE ---

    return SortedList;
}

void AMarbleGameMode::StartRace(int32 NumberOfMarbles)
{
    ResetRaceState();

    TotalMarbles = NumberOfMarbles;
    bRaceActive = true;
    RaceStartTime = GetWorld()->GetTimeSeconds();

    UE_LOG(LogTemp, Warning, TEXT("RENNEN GESTARTET! Erwarte %d Murmeln im Ziel."), TotalMarbles);
}

void AMarbleGameMode::ResetRaceState()
{
    bRaceActive = false;
    FinishedCount = 0;
    EliminatedCount = 0;
    RaceEndTime = 0.0f;
    RacingMarbles.Empty();
    ADataTracker::ClearAllDataSets();
}

float AMarbleGameMode::GetCurrentRaceTime() const
{
    if (bRaceActive)
    {
        return GetWorld()->GetTimeSeconds() - RaceStartTime;
    }
    if (RaceEndTime > 0.0f)
    {
        return RaceEndTime - RaceStartTime;
    }
    return 0.0f;
}

void AMarbleGameMode::RegisterMarbleFinished()
{
    if (!bRaceActive) return;

    FinishedCount++;
    UE_LOG(LogTemp, Log, TEXT("Murmel im Ziel. Fortschritt: %d/%d"), (FinishedCount + EliminatedCount), TotalMarbles);
    
    CheckRaceStatus();
}

void AMarbleGameMode::RegisterMarbleEliminated()
{
    if (!bRaceActive) return;

    EliminatedCount++;
    UE_LOG(LogTemp, Warning, TEXT("Murmel eliminiert. Fortschritt: %d/%d"), (FinishedCount + EliminatedCount), TotalMarbles);
    
    CheckRaceStatus();
}

void AMarbleGameMode::CheckRaceStatus()
{
    if ((FinishedCount + EliminatedCount) >= TotalMarbles)
    {
        bRaceActive = false;
        RaceEndTime = GetWorld()->GetTimeSeconds();

        UE_LOG(LogTemp, Warning, TEXT("RENNEN BEENDET! Alle Murmeln sind durch."));
        GetMarblesSortedByRank();
        RaceEnded();
        ADog* Dog = Cast<ADog>(UGameplayStatics::GetActorOfClass(this, ADog::StaticClass()));
        Dog->StopShockLoop();

        AMarblePlayerController* PC = Cast<AMarblePlayerController>(GetWorld()->GetFirstPlayerController());
        if (PC)
        {
            PC->SetRaceState(false);
            PC->SwitchToAnalysView();
            PC->SelectMarble(NULL);
        }
        
        if (OnRaceEnded.IsBound())
        {
            OnRaceEnded.Broadcast();
        }
    }
}

void AMarbleGameMode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (bRaceActive)
    {
        CurrentRaceTime += DeltaSeconds;
    }
}