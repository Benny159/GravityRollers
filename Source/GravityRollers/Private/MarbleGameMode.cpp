#include "MarbleGameMode.h"

AMarbleGameMode::AMarbleGameMode()
{
    bRaceActive = false;
    RaceStartTime = 0.0f;
    RaceEndTime = 0.0f;
    
    TotalMarbles = 5;
    FinishedCount = 0;
    EliminatedCount = 0;
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
        
        if (OnRaceEnded.IsBound())
        {
            OnRaceEnded.Broadcast();
        }
    }
}