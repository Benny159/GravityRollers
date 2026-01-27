#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Marble.h"
#include "Dog.generated.h"

UENUM(BlueprintType)
enum class EDogState : uint8
{
    Sleeping    UMETA(DisplayName = "Schlafen"),
    Idle        UMETA(DisplayName = "Idle"),
    Jumping     UMETA(DisplayName = "Sprung")
};

UCLASS()
class GRAVITYROLLERS_API ADog : public AActor
{
    GENERATED_BODY()
    
public:    
    ADog();

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Seismic Settings")
    bool bIsDogEnabled = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Seismic Settings")
    float MeanTimeBetweenShocks = 20.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Seismic Settings")
    float ShockStrength = 500.0f;

    UPROPERTY(EditAnywhere, Category = "Seismic Settings")
    TSubclassOf<UCameraShakeBase> SeismicShakeClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    EDogState CurrentState = EDogState::Sleeping;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USkeletalMeshComponent* DogMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UAudioComponent* BarkAudio;
    
    UFUNCTION(BlueprintCallable, Category = "Race Logic")
    void StartShockLoop();
    
    UFUNCTION(BlueprintCallable, Category = "Race Logic")
    void StopShockLoop();
    
    UFUNCTION(BlueprintCallable, Category = "Menu Logic")
    void SetDogEnabled(bool bEnabled);

private:
    FTimerHandle TimerHandle_NextShock;
    FTimerHandle TimerHandle_ResetAnim;
    FTimerHandle TimerHandle_ShockDelay;
    
    void ApplyShockEffects();
    
    void ScheduleNextShock();
    void PerformShock();
    void ResetToIdle();

    float GetExponentialRandom(float Mean);
    
    UPROPERTY()
    TArray<AMarble*> CachedMarbles;
    void UpdateMarbleCache();
};