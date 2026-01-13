#include "Fan.h"
#include "Components/ArrowComponent.h"
#include "Marble.h"

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
}

void AFan::BeginPlay()
{
    Super::BeginPlay();
}

void AFan::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bFanOn)
    {
        if (RotorMesh)
        {
            RotorMesh->AddLocalRotation(FRotator(0, RotationSpeed * DeltaTime, 0));
        }
        
        FVector WindDir = WindDirectionArrow->GetForwardVector();
        
        int32 ActiveMarbleCount = 0;

        for (AMarble* Marble : ActiveMarbles)
        {
            if (!IsValid(Marble)) continue;
            if (Marble->bHasFinished || Marble->bIsEliminated) continue;
            if (!Marble->ActorHasTag("RaceMarble")) continue;
            
            UStaticMeshComponent* MarbleMeshComp = Marble->GetMesh();
            
            if (MarbleMeshComp && MarbleMeshComp->IsSimulatingPhysics())
            {
                float RandomForce = GetGaussianRandom(MeanWindStrength, WindVariance);
                
                MarbleMeshComp->AddForce(WindDir * RandomForce);
                ActiveMarbleCount++;
            }
        }
    }
}

float AFan::GetGaussianRandom(float Mean, float StdDev)
{
    float u1 = FMath::FRand();
    float u2 = FMath::FRand();
    
    if (u1 <= 0) u1 = 0.0001f;
    
    float z0 = FMath::Sqrt(-2.0f * FMath::Loge(u1)) * FMath::Cos(2.0f * UE_PI * u2);
    
    return Mean + (z0 * StdDev);
}

void AFan::ToggleFan()
{
    bFanOn = !bFanOn;
}