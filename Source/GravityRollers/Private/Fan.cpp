// Copyright (c) 2026 Gravity Rollers. All Rights Reserved.

#include "Fan.h"
#include "Marble.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"

AFan::AFan()
{
    PrimaryActorTick.bCanEverTick = true;

    BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
    RootComponent = BaseMesh;

    RotorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RotorMesh"));
    RotorMesh->SetupAttachment(BaseMesh);

    WindDirectionArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("WindDir"));
    WindDirectionArrow->SetupAttachment(BaseMesh);
    WindDirectionArrow->ArrowSize = 2.0f;

    // Default values
    bIsFanActive = true;
    MeanWindStrength = 1000.0f;
    WindVariance = 250.0f;
    RotationSpeed = 800.0f;
}

void AFan::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsFanActive)
    {
        return;
    }
    
    if (RotorMesh)
    {
        RotorMesh->AddLocalRotation(FRotator(0.0f, RotationSpeed * DeltaTime, 0.0f));
    }
	
    const FVector WindDir = WindDirectionArrow->GetForwardVector();

    for (AMarble* Marble : ActiveMarbles)
    {
        if (!IsValid(Marble) || Marble->bHasFinished || Marble->bIsEliminated || !Marble->ActorHasTag("RaceMarble"))
        {
            continue;
        }

        UStaticMeshComponent* MarbleMeshComp = Marble->GetMesh();

        if (MarbleMeshComp && MarbleMeshComp->IsSimulatingPhysics())
        {
            const float RandomForce = GetGaussianRandom(MeanWindStrength, WindVariance);
			
            MarbleMeshComp->AddForce(WindDir * RandomForce);
        }
    }
}

float AFan::GetGaussianRandom(float Mean, float StdDev) const
{
    float U1 = FMath::FRand();
    float U2 = FMath::FRand();
    
    // Prevent Log(0)
    if (U1 <= 0.0f)
    {
        U1 = 0.0001f;
    }
    
    float Z0 = FMath::Sqrt(-2.0f * FMath::Loge(U1)) * FMath::Cos(2.0f * UE_PI * U2);
    
    return Mean + (Z0 * StdDev);
}

void AFan::ToggleFan()
{
    bIsFanActive = !bIsFanActive;
}