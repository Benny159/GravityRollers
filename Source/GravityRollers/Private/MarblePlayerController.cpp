#include "MarblePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "Marble.h"

AMarblePlayerController::AMarblePlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
    bRaceIsActive = false;
}

void AMarblePlayerController::BeginPlay()
{
    Super::BeginPlay();
    SwitchToConfigView();
}

void AMarblePlayerController::SetRaceState(bool bActive)
{
    bRaceIsActive = bActive;
}

void AMarblePlayerController::SwitchToRaceView()
{
    AActor* RaceCam = FindCameraByTag(FName("RaceCam"));
    if (RaceCam)
    {
        SetViewTargetWithBlend(RaceCam, 1.5f, EViewTargetBlendFunction::VTBlend_EaseInOut, 2.0f);
    }
}

void AMarblePlayerController::SwitchToConfigView()
{
    bRaceIsActive = false;
    AActor* ConfigCam = FindCameraByTag(FName("ConfigCam"));
    if (ConfigCam)
    {
        SetViewTargetWithBlend(ConfigCam, 1.0f, EViewTargetBlendFunction::VTBlend_EaseInOut, 2.0f);
    }
}

void AMarblePlayerController::FocusOnMarble(int32 MarbleIndex)
{
    if (!bRaceIsActive) return;
    
    TArray<AActor*> FoundMarbles;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("RaceMarble"), FoundMarbles);

    AMarble* TargetMarble = nullptr;
    
    for (AActor* Actor : FoundMarbles)
    {
        AMarble* M = Cast<AMarble>(Actor);
        if (M && M->StartingLaneIndex == MarbleIndex)
        {
            TargetMarble = M;
            break;
        }
    }
    
    if (TargetMarble)
    {
        SetViewTargetWithBlend(TargetMarble, 0.5f, EViewTargetBlendFunction::VTBlend_Cubic);
    }
}

AActor* AMarblePlayerController::FindCameraByTag(FName Tag)
{
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), Tag, FoundActors);

    if (FoundActors.Num() > 0)
    {
        return FoundActors[0];
    }
    
    UE_LOG(LogTemp, Warning, TEXT("WARNUNG: Kamera mit Tag '%s' nicht gefunden!"), *Tag.ToString());
    return nullptr;
}