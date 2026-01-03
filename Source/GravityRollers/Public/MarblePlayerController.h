#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Marble.h"
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

	UFUNCTION(BlueprintCallable, Category = "Camera Control")
	void FocusOnMarble(int32 MarbleIndex);

	UFUNCTION(BlueprintCallable, Category = "Game State")
	void SetRaceState(bool bActive);
	
	UFUNCTION(BlueprintPure, Category = "Game State")
	bool IsRaceActive() const { return bRaceIsActive; }

	void SelectMarble(AMarble* NewMarble);
	
	UPROPERTY(BlueprintReadOnly)
	AMarble* CurrentSelectedMarble;

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnShowMarbleUI(AMarble* Marble);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnHideMarbleUI();

private:
	bool bRaceIsActive;

	int32 CurrentViewIndex;
    
	AActor* FindCameraByTag(FName Tag);

	void ViewMarble1();
	void ViewMarble2();
	void ViewMarble3();
	void ViewMarble4();
	void ViewMarble5();

	void CycleToNextMarble();
};
