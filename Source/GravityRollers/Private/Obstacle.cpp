#include "Obstacle.h"

AObstacle::AObstacle()
{
	PrimaryActorTick.bCanEverTick = false;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObstacleMesh"));
    RootComponent = Mesh;
    
    Mesh->SetCollisionProfileName(TEXT("BlockAllDynamic")); 
    Mesh->SetMobility(EComponentMobility::Movable);
}
