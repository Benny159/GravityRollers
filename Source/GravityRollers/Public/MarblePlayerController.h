#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Marble.h"
#include "Camera/CameraActor.h"
#include "MarblePlayerController.generated.h"

UCLASS()
class GRAVITYROLLERS_API AMarblePlayerController : public APlayerController
{
	GENERATED_BODY()
    
public:
	AMarblePlayerController();

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Camera Control")
	void SwitchToRaceView();
	
	UFUNCTION(BlueprintCallable, Category = "Camera Control")
	void SwitchToConfigView();
	
	void SwitchToConfigViewFromRace(FVector LastCameraLocation, FRotator LastCameraRotation);

	UFUNCTION(BlueprintCallable, Category = "Camera Control")
	void SwitchToAnalysView();

	UFUNCTION(BlueprintCallable, Category = "Camera Control")
	void SwitchToNextActiveMarble();

	UFUNCTION(BlueprintCallable, Category = "Camera Control")
	void FocusOnMarble(int32 MarbleIndex);

	UFUNCTION(BlueprintCallable, Category = "Game State")
	void SetRaceState(bool bActive);
	
	UFUNCTION(BlueprintPure, Category = "Game State")
	bool IsRaceActive() const { return bRaceIsActive; }

	UFUNCTION(BlueprintCallable, Category = "Game State")
	void SelectMarble(AMarble* NewMarble);
	
	UPROPERTY(BlueprintReadOnly)
	AMarble* CurrentSelectedMarble;

	UPROPERTY(BlueprintReadOnly)
	AMarble* CurrentViewedMarble;

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnShowMarbleUI(AMarble* Marble);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnHideMarbleUI();

private:
	UPROPERTY()
	ACameraActor* GhostCameraActor;

	FTimerHandle TimerHandle_CleanupGhost;

	UFUNCTION()
	void CleanupGhostCamera();
	
	bool bRaceIsActive;

	int32 CurrentViewIndex;
    
	AActor* FindCameraByTag(FName Tag);

	void ViewMarble1();
	void ViewMarble2();
	void ViewMarble3();
	void ViewMarble4();
	void ViewMarble5();

	void CycleToNextMarble();
	
	bool bIsCameraSwitching = false;
	
	FTimerHandle TimerHandle_CameraCooldown;
	
	void UnlockCameraSwitch();
};
