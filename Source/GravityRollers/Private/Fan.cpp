#include "Fan.h"
#include "MarbleGameMode.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
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

static float FanLogTimer = 0.0f; 

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
        
        FanLogTimer += DeltaTime;
        bool bShouldLog = FanLogTimer >= 1.0f;
        
        if (bShouldLog)
        {
            FanLogTimer = 0.0f;
            UE_LOG(LogTemp, Warning, TEXT("--- FAN STATUS BERICHT ---"));
            UE_LOG(LogTemp, Warning, TEXT("Fan ist AN. GameMode kennt %d Murmeln."), ActiveMarbles.Num());
        }

        int32 ActiveMarbleCount = 0;

        for (AMarble* Marble : ActiveMarbles)
        {
            if (!IsValid(Marble)) continue;
            if (Marble->bHasFinished || Marble->bIsEliminated) continue;
            
            if (!Marble->ActorHasTag("RaceMarble")) 
            {
                if(bShouldLog) UE_LOG(LogTemp, Warning, TEXT("Ignoriere %s (Kein 'RaceMarble' Tag)"), *Marble->GetName());
                continue;
            }
            
            UStaticMeshComponent* MarbleMeshComp = Marble->GetMesh();
            
            if (MarbleMeshComp && MarbleMeshComp->IsSimulatingPhysics())
            {
                float RandomForce = GetGaussianRandom(MeanWindStrength, WindVariance);
                
                MarbleMeshComp->AddForce(WindDir * RandomForce);
                ActiveMarbleCount++;

                if (bShouldLog)
                {
                    UE_LOG(LogTemp, Log, TEXT("Wind wirkt auf: %s | Kraft: %f"), *Marble->GetName(), RandomForce);
                }
            }
        }

        
        if(bShouldLog)
        {
            UE_LOG(LogTemp, Warning, TEXT("Insgesamt %d Murmeln vom Wind erfasst."), ActiveMarbleCount);
            UE_LOG(LogTemp, Warning, TEXT("--------------------------"));
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
    
    UE_LOG(LogTemp, Warning, TEXT("Fan wurde umgeschaltet. Neuer Status: %s"), bFanOn ? TEXT("AN") : TEXT("AUS"));
}