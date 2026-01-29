// Copyright (c) 2026 Gravity Rollers. All Rights Reserved.

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

    ObstacleCount = 10;
    MinDistance = 250;
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
        return;
    }

    FCollisionQueryParams TraceParams;
    TraceParams.AddIgnoredActor(this);
    TraceParams.AddIgnoredActors(SpawnedObstacles);
    
    for (int32 i = 0; i < ObstacleCount; i++)
    {
        AttemptSpawnSingleObstacle(TraceParams);
    }
}

void AObstacleGenerator::AttemptSpawnSingleObstacle(FCollisionQueryParams& TraceParams)
{
    bSpawned = false;
    int32 Attempts = 0;
    
    while (Attempts < 15 && !bSpawned)
    {
        FHitResult HitResult = GenerateRandomHitResult(TraceParams);
        if (bHit)
        {
            if (!IsBlocked(HitResult))
            {
                PlaceSingleObstacle(HitResult);
            }
        }
        Attempts++;
    }
}

FHitResult AObstacleGenerator:: GenerateRandomHitResult(FCollisionQueryParams& TraceParams)
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
    SetHit(HitResult, TraceStart, TraceEnd, TraceParams);

    return HitResult;
}

void AObstacleGenerator::SetHit(FHitResult& HitResult, FVector TraceStart, FVector TraceEnd, FCollisionQueryParams& TraceParams)
{
    bool bCollsionHit = GetWorld()->LineTraceSingleByChannel(
     HitResult,
     TraceStart,
     TraceEnd,
     ECC_WorldStatic,
     TraceParams
    );
    bHit = bCollsionHit;
}

bool AObstacleGenerator::IsBlocked(FHitResult& HitResult)
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

    return bIsBlocked;
}

void AObstacleGenerator::PlaceSingleObstacle(FHitResult& HitResult)
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
