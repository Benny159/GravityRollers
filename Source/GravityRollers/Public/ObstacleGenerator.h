#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Obstacle.h"
#include "ObstacleGenerator.generated.h"

UCLASS()
class GRAVITYROLLERS_API AObstacleGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	AObstacleGenerator();

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* SpawnArea;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
	TArray<TSubclassOf<AObstacle>> ObstacleTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
	int32 ObstacleCount = 10;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
	float MinDistance = 250.0f;
	
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Generation")
	void GenerateObstacles();
	
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Generation")
	void ClearObstacles();

private:
	UPROPERTY()
	TArray<AActor*> SpawnedObstacles;
};