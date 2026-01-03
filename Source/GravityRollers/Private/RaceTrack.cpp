#include "RaceTrack.h"
#include "MarbleGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "MarblePlayerController.h"

ARaceTrack::ARaceTrack()
{
    PrimaryActorTick.bCanEverTick = false;
    
    TrackMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrackMesh"));
    RootComponent = TrackMesh;
    
    StartBarrier = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StartBarrier"));
    StartBarrier->SetupAttachment(RootComponent);
    StartBarrier->SetMobility(EComponentMobility::Movable);
    
    EndTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("EndTrigger"));
    EndTrigger->SetupAttachment(RootComponent);
    EndTrigger->SetBoxExtent(FVector(100.f, 500.f, 100.f));
    EndTrigger->SetCollisionProfileName(TEXT("Trigger"));
    
    StartPointsRoot = CreateDefaultSubobject<USceneComponent>(TEXT("StartPointsRoot"));
    StartPointsRoot->SetupAttachment(RootComponent);

    for (int32 i = 0; i < 5; i++)
    {
        FString Name = FString::Printf(TEXT("StartPosition_%d"), i + 1);
        UArrowComponent* Arrow = CreateDefaultSubobject<UArrowComponent>(*Name);
        Arrow->SetupAttachment(StartPointsRoot);
        
        Arrow->SetRelativeLocation(FVector(0, i * 150.0f, 50.0f)); 
        StartPositions.Add(Arrow);
    }

    bRaceStarted = false;
}

void ARaceTrack::BeginPlay()
{
    Super::BeginPlay();
    
    RegisterCheckpoints();
    
    if (EndTrigger)
    {
        EndTrigger->OnComponentBeginOverlap.AddDynamic(this, &ARaceTrack::OnEndOverlap);
    }
}

void ARaceTrack::RegisterCheckpoints()
{
    Checkpoints.Empty();
    
    TArray<UChildActorComponent*> ChildComponents;
    GetComponents<UChildActorComponent>(ChildComponents);

    for (UChildActorComponent* ChildComp : ChildComponents)
    {
        ACheckpoint* FoundCheckpoint = Cast<ACheckpoint>(ChildComp->GetChildActor());
        if (FoundCheckpoint)
        {
            Checkpoints.Add(FoundCheckpoint);
        }
    }
    
    Checkpoints.Sort([](const ACheckpoint& A, const ACheckpoint& B) {
        return A.GetName() < B.GetName();
    });
    
    for (int32 i = 0; i < Checkpoints.Num(); i++)
    {
        if (Checkpoints[i])
        {
            Checkpoints[i]->SetCheckpointIndex(i);
        }
    }
}

void ARaceTrack::SpawnMarble(TSubclassOf<AMarble> MarbleClass, int32 LaneIndex, const FMarbleData& MarbleData)
{
    if (!MarbleClass || !StartPositions.IsValidIndex(LaneIndex)) return;

    FTransform SpawnTransform = StartPositions[LaneIndex]->GetComponentTransform();
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AMarble* NewMarble = GetWorld()->SpawnActor<AMarble>(MarbleClass, SpawnTransform, SpawnParams);

    if (NewMarble)
    {
        NewMarble->InitializeFromData(MarbleData);
        NewMarble->Tags.Add(FName("RaceMarble"));
        
        NewMarble->SetFrozen(true); 

        ActiveMarbles.Add(NewMarble);
    }
}

void ARaceTrack::SetupRaceFromData(const TArray<FMarbleData>& MarblesData)
{
    ResetTrack();

    for (const FMarbleData& Data : MarblesData)
    {
        int32 TargetLane = Data.PreferredLaneIndex;
        
        if (StartPositions.IsValidIndex(TargetLane))
        {
            SpawnMarble(RaceMarbleClass, TargetLane, Data); 
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Ungültiger LaneIndex in Daten: %d"), TargetLane);
        }
    }
    // Optional: Hier könnte man die Kamera initialisieren
}

void ARaceTrack::StartRace()
{
    if (bRaceStarted) return;
    bRaceStarted = true;
    
    StartBarrier->SetRelativeRotation(FRotator(80.f, 0.f, 0.f));

    for (AMarble* Marble : ActiveMarbles)
    {
        if (Marble)
        {
            Marble->SetFrozen(false);
        }
    }

    AMarblePlayerController* PC = Cast<AMarblePlayerController>(GetWorld()->GetFirstPlayerController());
    if (PC)
    {
        PC->SetRaceState(true);
        PC->FocusOnMarble(0); 
    }
    
    AMarbleGameMode* GM = Cast<AMarbleGameMode>(UGameplayStatics::GetGameMode(this));
    if (GM)
    {
        GM->StartRace(ActiveMarbles.Num());
    }
}

void ARaceTrack::ResetTrack()
{
    bRaceStarted = false;

    AMarblePlayerController* PC = Cast<AMarblePlayerController>(GetWorld()->GetFirstPlayerController());
    if (PC)
    {
        PC->SetRaceState(false);
        PC->SwitchToConfigView();
    }
    
    for (AMarble* Marble : ActiveMarbles)
    {
        if (Marble && IsValid(Marble))
        {
            Marble->Destroy();
        }
    }
    ActiveMarbles.Empty();
    
    StartBarrier->SetRelativeRotation(FRotator::ZeroRotator);

    AMarbleGameMode* GM = Cast<AMarbleGameMode>(UGameplayStatics::GetGameMode(this));
    if (GM)
    {
        GM->ResetRaceState();
    }

    UE_LOG(LogTemp, Log, TEXT("RaceTrack zurückgesetzt."));
}

void ARaceTrack::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AMarble* Marble = Cast<AMarble>(OtherActor);
    
    if (Marble && !Marble->bHasFinished)
    {
        float FinishTime = 0.0f;
        AMarbleGameMode* GM = Cast<AMarbleGameMode>(UGameplayStatics::GetGameMode(this));
        
        if (GM) FinishTime = GM->GetCurrentRaceTime();
        else FinishTime = GetWorld()->GetTimeSeconds();

        float FinishSpeed = Marble->GetVelocity().Size();
        
        Marble->FinishRace(FinishTime, FinishSpeed);
        
        if (GM)
        {
            GM->RegisterMarbleFinished();
        }
    }
}

