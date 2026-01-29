#include "MarbleGameMode.h"
#include "DataTrackerPlugin/DataTracker.h"
#include "Dog.h"
#include "Kismet/GameplayStatics.h"
#include "MarblePlayerController.h"

AMarbleGameMode::AMarbleGameMode()
{
    bAllowWind = true;
	bAllowSeismic = true;
	bRaceActive = false;
	RaceStartTime = 0.0f;
	RaceEndTime = 0.0f;
	CurrentRaceTime = 0.0f;
	TotalMarbles = 5;
	FinishedCount = 0;
	EliminatedCount = 0;
}

void AMarbleGameMode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (bRaceActive)
    {
        CurrentRaceTime += DeltaSeconds;
    }
}

void AMarbleGameMode::StartRace(int32 NumberOfMarbles)
{
    ResetRaceState();

    TotalMarbles = NumberOfMarbles;
    bRaceActive = true;
    RaceStartTime = GetWorld()->GetTimeSeconds();
}

void AMarbleGameMode::RegisterMarble(AMarble* NewMarble)
{
    if (NewMarble && !RacingMarbles.Contains(NewMarble))
    {
        RacingMarbles.Add(NewMarble);
    }
}

TArray<AMarble*> AMarbleGameMode::GetMarblesSortedByRank()
{
    RacingMarbles.RemoveAll([](AMarble* Marble) {
        return Marble == nullptr || !IsValid(Marble);
    });

    TArray<AMarble*> SortedList = RacingMarbles;
    
    SortedList.Sort([](const AMarble& A, const AMarble& B)
    {
        return A.FinalRank < B.FinalRank;
    });

    return SortedList;
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
    if (!bRaceActive)
	{
		return;
	}

    FinishedCount++;
    
    CheckRaceStatus();
}

void AMarbleGameMode::RegisterMarbleEliminated()
{
    if (!bRaceActive)
	{
		return;
	}

    EliminatedCount++;
    
    CheckRaceStatus();
}

void AMarbleGameMode::CheckRaceStatus()
{
    if ((FinishedCount + EliminatedCount) >= TotalMarbles)
    {
        bRaceActive = false;
        RaceEndTime = GetWorld()->GetTimeSeconds();

        GetMarblesSortedByRank();
        RaceEnded();
        if (ADog* Dog = Cast<ADog>(UGameplayStatics::GetActorOfClass(this, ADog::StaticClass())))
		{
			Dog->StopShockLoop();
		}

        if (AMarblePlayerController* PC = Cast<AMarblePlayerController>(GetWorld()->GetFirstPlayerController()))
		{
			PC->SetRaceState(false);
			PC->SwitchToAnalysView();
			PC->SelectMarble(nullptr);
		}
        
        if (OnRaceEnded.IsBound())
        {
            OnRaceEnded.Broadcast();
        }
    }
}

void AMarbleGameMode::ForceEndRace()
{
    if (!bRaceActive)
	{
		return;
	}

    TArray<AActor*> FoundMarbles;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("RaceMarble"), FoundMarbles);

    for (AActor* Actor : FoundMarbles)
    {
        if (AMarble* Marble = Cast<AMarble>(Actor))
        {
            if (IsValid(Marble) && !Marble->bHasFinished && !Marble->bIsEliminated)
            {
                Marble->Eliminate();
                
                // Assign a low rank to indicate DNF (Did Not Finish) or Forced End
                Marble->FinalRank = 99;
                
                RegisterMarble(Marble);
                RegisterMarbleEliminated();
            }
        }
    }
}
