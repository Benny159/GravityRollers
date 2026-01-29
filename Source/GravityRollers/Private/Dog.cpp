// Copyright (c) 2026 Gravity Rollers. All Rights Reserved.

#include "Dog.h"
#include "Marble.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Camera/CameraShakeBase.h"
#include "TimerManager.h"

ADog::ADog()
{
    PrimaryActorTick.bCanEverTick = false;

    DogMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DogMesh"));
    RootComponent = DogMesh;

    // Default values
    bIsDogEnabled = true;
    MeanTimeBetweenShocks = 20.0f;
    ShockStrength = 500.0f;
    CurrentState = EDogState::Sleeping;
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
    }
    else
    {
        CurrentState = EDogState::Sleeping;
        
        StopShockLoop();
    }
}


void ADog::StartShockLoop()
{
    if (bIsDogEnabled)
    {
        ScheduleNextShock();
    }
}

void ADog::StopShockLoop()
{
    GetWorld()->GetTimerManager().ClearTimer(TimerHandle_NextShock);
    GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ResetAnim);
    GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ShockDelay);
    
    if (bIsDogEnabled)
    {
        CurrentState = EDogState::Idle;
    }
}

void ADog::ScheduleNextShock()
{
    if (!bIsDogEnabled)
    {
        return;
    }

    const float Delay = GetExponentialRandom(MeanTimeBetweenShocks);
    
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
    if (!bIsDogEnabled)
    {
        return;
    }
    
    CurrentState = EDogState::Jumping;

    // Delay the actual physical impact slightly to match animation
    GetWorld()->GetTimerManager().SetTimer(
        TimerHandle_ShockDelay, 
        this, 
        &ADog::ApplyShockEffects, 
        0.5f,
        false
    );

    // Schedule reset to idle
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
        if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
        {
            PC->ClientStartCameraShake(SeismicShakeClass, 1.0f);
        }
    }
    
    UpdateMarbleCache();
    
    for (AMarble* Marble : CachedMarbles)
    {
        if (IsValid(Marble) && Marble->GetMesh() && Marble->GetMesh()->IsSimulatingPhysics())
        {
            FVector ShakeDir = FMath::VRand();
            ShakeDir.Z = 2.5f;
            ShakeDir.Normalize();
            
            Marble->GetMesh()->AddImpulse(ShakeDir * ShockStrength, NAME_None, true);
        }
    }
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
    if (Mean <= 0.0f)
    {
        return 5.0f;
    }
    float U = FMath::FRand(); 
    // Clamp to avoid Log(0)
    if (U <= 0.0001f)
    {
        U = 0.0001f;
    }
    const float ExpTime = -FMath::Loge(U) * Mean;
    // Minimum 2 seconds delay
    return 2.0f + ExpTime;
}

void ADog::UpdateMarbleCache()
{
    CachedMarbles.Empty();
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMarble::StaticClass(), FoundActors);
    for (AActor* Actor : FoundActors)
    {
        if (AMarble* Marble = Cast<AMarble>(Actor))
        {
            if (IsValid(Marble) && !Marble->bHasFinished && !Marble->bIsEliminated)
            {
                CachedMarbles.Add(Marble);
            }
        }
    }
}