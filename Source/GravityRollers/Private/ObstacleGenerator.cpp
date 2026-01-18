#include "ObstacleGenerator.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

AObstacleGenerator::AObstacleGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

    SpawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnArea"));
    RootComponent = SpawnArea;
    
    SpawnArea->SetBoxExtent(FVector(1000.f, 500.f, 100.f)); 
    SpawnArea->SetLineThickness(5.0f);
    SpawnArea->ShapeColor = FColor::Orange;
}

void AObstacleGenerator::BeginPlay()
{
	Super::BeginPlay();
    GenerateObstacles();
}

void AObstacleGenerator::GenerateObstacles()
{
    ClearObstacles();

    if (ObstacleTypes.Num() == 0) 
    {
        UE_LOG(LogTemp, Warning, TEXT("Generator Fehler: Keine ObstacleTypes im Array hinterlegt!"));
        return;
    }

    FCollisionQueryParams TraceParams;
    TraceParams.AddIgnoredActor(this);
    TraceParams.AddIgnoredActors(SpawnedObstacles);

    FCollisionQueryParams OverlapParams;
    OverlapParams.AddIgnoredActor(this);
    
    for (int32 i = 0; i < ObstacleCount; i++)
    {
        bool bSpawned = false;
        int32 Attempts = 0;
        
        while (Attempts < 15 && !bSpawned)
        {
            FVector BoxOrigin = SpawnArea->GetComponentLocation();
            FVector BoxExtent = SpawnArea->GetScaledBoxExtent();
            FRotator BoxRotation = SpawnArea->GetComponentRotation();
            
            FVector RandomLocalPoint;
            RandomLocalPoint.X = FMath::RandRange(-BoxExtent.X, BoxExtent.X);
            RandomLocalPoint.Y = FMath::RandRange(-BoxExtent.Y, BoxExtent.Y);
            RandomLocalPoint.Z = FMath::RandRange(-BoxExtent.Z, BoxExtent.Z);

            FVector RandomPoint = BoxOrigin + BoxRotation.RotateVector(RandomLocalPoint);
            
            FVector TraceStart = RandomPoint;
            FVector TraceEnd = RandomPoint - (SpawnArea->GetUpVector() * (BoxExtent.Z * 4.0f));

            FHitResult HitResult;
            bool bHit = GetWorld()->LineTraceSingleByChannel(
                HitResult,
                TraceStart,
                TraceEnd,
                ECC_WorldStatic,
                TraceParams
            );

            if (bHit)
            {
                FCollisionQueryParams OverlapParamsHit;
                OverlapParamsHit.AddIgnoredActor(this);
                
                if (HitResult.GetActor())
                {
                    OverlapParamsHit.AddIgnoredActor(HitResult.GetActor());
                }
                
                float CheckHeightOffset = MinDistance * 0.5f; 
                FVector CheckLoc = HitResult.Location + (HitResult.ImpactNormal * (CheckHeightOffset + 2.0f));

                bool bIsBlocked = GetWorld()->OverlapAnyTestByChannel(
                    CheckLoc,
                    FQuat::Identity,
                    ECC_WorldDynamic,
                    FCollisionShape::MakeSphere(MinDistance * 0.9f),
                    OverlapParamsHit
                );
                if (!bIsBlocked)
                {
                    int32 TypeIndex = FMath::RandRange(0, ObstacleTypes.Num() - 1);
                    TSubclassOf<AObstacle> SelectedClass = ObstacleTypes[TypeIndex];

                    if (SelectedClass)
                    {
                        FRotator SurfaceRot = UKismetMathLibrary::MakeRotFromZ(HitResult.ImpactNormal);
                        float RandomYaw = FMath::RandRange(0.0f, 360.0f);
                        SurfaceRot = UKismetMathLibrary::ComposeRotators(FRotator(0, RandomYaw, 0), SurfaceRot);

                        FActorSpawnParameters SpawnParams;
                        SpawnParams.Owner = this;
                        
                        AActor* NewObs = GetWorld()->SpawnActor<AActor>(SelectedClass, HitResult.Location, SurfaceRot, SpawnParams);
                        
                        if (NewObs)
                        {
                            NewObs->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
                            SpawnedObstacles.Add(NewObs);
                            bSpawned = true;
                        }
                    }
                }
            }
            Attempts++;
        }
    }
}

void AObstacleGenerator::ClearObstacles()
{
    for (AActor* Ob : SpawnedObstacles)
    {
        if (IsValid(Ob))
        {
            Ob->Destroy();
        }
    }
    SpawnedObstacles.Empty();
}

