#include "Dog.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Camera/CameraShakeBase.h"

ADog::ADog()
{
    PrimaryActorTick.bCanEverTick = false;

    DogMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DogMesh"));
    RootComponent = DogMesh;

    BarkAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("BarkAudio"));
    BarkAudio->SetupAttachment(DogMesh);
    BarkAudio->bAutoActivate = false;
}

void ADog::BeginPlay()
{
    Super::BeginPlay();
    
    if (bIsDogEnabled)
    {
        CurrentState = EDogState::Idle;
    }
    else
    {
        CurrentState = EDogState::Sleeping;
    }
}

void ADog::SetDogEnabled(bool bEnabled)
{
    bIsDogEnabled = bEnabled;

    if (bIsDogEnabled)
    {
        CurrentState = EDogState::Idle;
        UE_LOG(LogTemp, Log, TEXT("Hund: Aktiviert (Steht auf)."));
    }
    else
    {
        CurrentState = EDogState::Sleeping;
        UE_LOG(LogTemp, Log, TEXT("Hund: Deaktiviert (Legt sich hin)."));
        
        StopShockLoop();
    }
}


void ADog::StartShockLoop()
{
    if (bIsDogEnabled)
    {
        UE_LOG(LogTemp, Warning, TEXT("Hund: Rennen startet -> Erdbeben-Timer aktiv!"));
        ScheduleNextShock();
    }
}

void ADog::StopShockLoop()
{
    GetWorld()->GetTimerManager().ClearTimer(TimerHandle_NextShock);
    GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ResetAnim);
    
    if (bIsDogEnabled)
    {
        CurrentState = EDogState::Idle;
    }
}

void ADog::ScheduleNextShock()
{
    if (!bIsDogEnabled) return;

    float Delay = GetExponentialRandom(MeanTimeBetweenShocks);
    
    GetWorld()->GetTimerManager().SetTimer(
        TimerHandle_NextShock, 
        this, 
        &ADog::PerformShock, 
        Delay, 
        false
    );
}

void ADog::PerformShock()
{
    if (!bIsDogEnabled) return;
    
    CurrentState = EDogState::Jumping;
    
    if (BarkAudio) BarkAudio->Play();
    
    GetWorld()->GetTimerManager().SetTimer(
        TimerHandle_ShockDelay, 
        this, 
        &ADog::ApplyShockEffects, 
        0.5f,
        false
    );

    GetWorld()->GetTimerManager().SetTimer(
        TimerHandle_ResetAnim, 
        this, 
        &ADog::ResetToIdle, 
        1.5f,
        false
    );
}

void ADog::ApplyShockEffects()
{
    if (SeismicShakeClass)
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
        if (PC)
        {
            PC->ClientStartCameraShake(SeismicShakeClass, 1.0f); 
        }
    }
    
    UpdateMarbleCache();
    int32 HitCount = 0;
    
    for (AMarble* Marble : CachedMarbles)
    {
        if (IsValid(Marble) && Marble->GetMesh() && Marble->GetMesh()->IsSimulatingPhysics())
        {
            FVector ShakeDir = FMath::VRand();
            ShakeDir.Z = 2.5f;
            ShakeDir.Normalize();
            
            Marble->GetMesh()->AddImpulse(ShakeDir * ShockStrength, NAME_None, true);
            HitCount++;
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Hund: WUMMS! (Erdbeben verzögert ausgelöst bei %d Murmeln)"), HitCount);
}

void ADog::ResetToIdle()
{

    if (bIsDogEnabled)
    {
        CurrentState = EDogState::Idle;

        ScheduleNextShock();
    }
}

float ADog::GetExponentialRandom(float Mean)
{
    if (Mean <= 0.0f) return 5.0f;
    float U = FMath::FRand(); 
    if (U <= 0.0001f) U = 0.0001f;
    float ExpTime = -FMath::Loge(U) * Mean;
    return 2.0f + ExpTime;
}

void ADog::UpdateMarbleCache()
{
    CachedMarbles.Empty();
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMarble::StaticClass(), FoundActors);
    for (AActor* Actor : FoundActors)
    {
        AMarble* Marble = Cast<AMarble>(Actor);
        if (Marble && Marble->ActorHasTag("RaceMarble") && !Marble->bHasFinished && !Marble->bIsEliminated)
        {
            CachedMarbles.Add(Marble);
        }
    }
}