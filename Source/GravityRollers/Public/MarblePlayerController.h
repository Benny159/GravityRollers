#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MarblePlayerController.generated.h"

UCLASS()
class GRAVITYROLLERS_API AMarblePlayerController : public APlayerController
{
	GENERATED_BODY()
    
public:
	AMarblePlayerController();

protected:
	virtual void BeginPlay() override;

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

private:
	bool bRaceIsActive;
    
	AActor* FindCameraByTag(FName Tag);
};
