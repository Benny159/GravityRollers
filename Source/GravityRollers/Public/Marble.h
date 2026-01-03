#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Marble.generated.h"

USTRUCT(BlueprintType)
struct FMarbleData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MarbleName = "Marble";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Size = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Weight = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SurfaceRoughness = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaterialDensity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector MassDistribution = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Friction = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Restitution = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AngularDamping = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PreferredLaneIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor MarbleColor = FLinearColor::White;
};

UCLASS()
class GRAVITYROLLERS_API AMarble : public AActor
{
	GENERATED_BODY()
	
public:
	AMarble();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorOnClicked(FKey ButtonPressed = EKeys::LeftMouseButton) override;
	
	UFUNCTION(BlueprintCallable)
	FMarbleData GetMarbleData() const;
	
	bool bIsSelected;
	FVector InitialLocation;
	void UpdateSelectionVisuals(float DeltaTime);
	void SetSelected(bool bSelected);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* MarbleMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* InnerCore;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UParticleSystemComponent* TrailEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Race Stats")
	int32 StartingLaneIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Race Stats")
	bool bHasFinished;
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Race Stats")
	bool bIsEliminated;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Race Stats")
	float FinalRaceTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Race Stats")
	float FinalRaceSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Race Stats")
	TArray<float> CheckpointTimes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Race Stats")
	TArray<float> CheckpointSpeeds;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CostumPhysics")
	float Size = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CostumPhysics")
	float Weight = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CostumPhysics")
	float SurfaceRoughness = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CostumPhysics")
	float MaterialDensity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CostumPhysics")
	FVector MassDistribution = FVector(0.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CostumPhysics")
	float Mass = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CostumPhysics")
	float Friction = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CostumPhysics")
	float Restitution = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CostumPhysics")
	float AngularDamping = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CostumPhysics")
	TEnumAsByte<EFrictionCombineMode::Type> FrictionCombineMode = EFrictionCombineMode::Min;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CostumPhysics")
	TEnumAsByte<EFrictionCombineMode::Type> RestitutionCombineMode = EFrictionCombineMode::Max;
	
	void UpdatePhysicsProperties();
	UPhysicalMaterial* CreatePhysicsMaterial();

	UFUNCTION(BlueprintCallable, Category="Configuration")
	void InitializeFromData(const FMarbleData& Data);

	UFUNCTION(BlueprintCallable, Category="Race Logic")
	void SetFrozen(bool bFrozen);
	
	void PassCheckpoint(int32 CheckpointIndex, float TimeStamp, float CurrentSpeed);
	
	void FinishRace(float TimeStamp, float FinishSpeed);
	
	UFUNCTION(BlueprintCallable, Category="Race Logic")
	void Eliminate();
};
