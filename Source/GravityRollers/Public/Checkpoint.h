#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Checkpoint.generated.h"

UCLASS()
class GRAVITYROLLERS_API ACheckpoint : public AActor
{
	GENERATED_BODY()
    
public:    
	ACheckpoint();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* TriggerBox;
	
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	//UStaticMeshComponent* CheckpointMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Race Logic")
	int32 CheckpointIndex;
	
	void SetCheckpointIndex(int32 NewIndex);
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};