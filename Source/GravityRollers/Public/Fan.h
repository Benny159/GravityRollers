#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Marble.h"
#include "Fan.generated.h"

class AMarbleGameMode;

UCLASS()
class GRAVITYROLLERS_API AFan : public AActor
{
	GENERATED_BODY()
	
public:	
	AFan();
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fan")
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fan")
	UStaticMeshComponent* RotorMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fan")
	class UArrowComponent* WindDirectionArrow;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wind Physics")
	bool bFanOn = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wind Physics")
	float MeanWindStrength = 1000.0f; 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wind Physics")
	float WindVariance = 250.0f;

	UPROPERTY(EditAnywhere, Category = "Visuals")
	float RotationSpeed = 800.0f;

	UFUNCTION(BlueprintCallable)
	void ToggleFan();

	UPROPERTY()
	TArray<AMarble*> ActiveMarbles;
	
	float GetGaussianRandom(float Mean, float StdDev);
};
