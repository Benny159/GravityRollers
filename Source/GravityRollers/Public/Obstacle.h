#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Obstacle.generated.h"

UCLASS()
class GRAVITYROLLERS_API AObstacle : public AActor
{
	GENERATED_BODY()
	
public:	
	AObstacle();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* Mesh;
};