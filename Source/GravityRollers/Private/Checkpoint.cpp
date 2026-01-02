#include "Checkpoint.h"
#include "Marble.h"
#include "MarbleGameMode.h"
#include "Kismet/GameplayStatics.h"

ACheckpoint::ACheckpoint()
{
    PrimaryActorTick.bCanEverTick = false;
    
    USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;
    
    //CheckpointMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CheckpointMesh"));
    //CheckpointMesh->SetupAttachment(RootComponent);
    //CheckpointMesh->SetCollisionProfileName(TEXT("NoCollision")); 
    
    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    TriggerBox->SetupAttachment(RootComponent);
    TriggerBox->SetBoxExtent(FVector(200.f, 50.f, 100.f));
    TriggerBox->SetCollisionProfileName(TEXT("Trigger"));

    CheckpointIndex = -1;
}

void ACheckpoint::BeginPlay()
{
    Super::BeginPlay();
    
    if (TriggerBox)
    {
        TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnOverlapBegin);
    }
}

void ACheckpoint::SetCheckpointIndex(int32 NewIndex)
{
    CheckpointIndex = NewIndex;
}

void ACheckpoint::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AMarble* Marble = Cast<AMarble>(OtherActor);

    if (Marble)
    {
        float CurrentRaceTime = 0.0f;
        AMarbleGameMode* GM = Cast<AMarbleGameMode>(UGameplayStatics::GetGameMode(this));
        
        if (GM) 
        {
            CurrentRaceTime = GM->GetCurrentRaceTime();
        }
        else 
        {
            CurrentRaceTime = GetWorld()->GetTimeSeconds();
        }
        
        float CurrentSpeed = Marble->GetVelocity().Size();
        
        Marble->PassCheckpoint(CheckpointIndex, CurrentRaceTime, CurrentSpeed);
    }
}