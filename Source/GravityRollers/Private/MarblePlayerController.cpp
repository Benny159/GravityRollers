#include "MarblePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "DataTrackerPlugin/DataTracker.h"
#include "Camera/CameraActor.h"
#include "Marble.h"

AMarblePlayerController::AMarblePlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
    bRaceIsActive = false;
    CurrentViewIndex = -1;
    CurrentSelectedMarble = nullptr;
}

void AMarblePlayerController::BeginPlay()
{
    Super::BeginPlay();
    SwitchToConfigView();
    ADataTracker::ClearAllDataSets();
}

void AMarblePlayerController::SelectMarble(AMarble* NewMarble)
{
    if (CurrentSelectedMarble == NewMarble)
    {
        if (CurrentSelectedMarble)
        {
            CurrentSelectedMarble->SetSelected(false);
        }
        
        CurrentSelectedMarble = nullptr;
        
        OnHideMarbleUI(); 
        
        return;
    }
    
    if (CurrentSelectedMarble)
    {
        CurrentSelectedMarble->SetSelected(false);
        
        OnHideMarbleUI(); 
    }
    
    CurrentSelectedMarble = NewMarble;
    
    if (CurrentSelectedMarble)
    {
        CurrentSelectedMarble->SetSelected(true);
        
        OnShowMarbleUI(CurrentSelectedMarble); 
        
        UE_LOG(LogTemp, Log, TEXT("UI angefordert für: %s"), *CurrentSelectedMarble->GetName());
    }
}

void AMarblePlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    
    if (InputComponent)
    {
        InputComponent->BindAction("ViewMarble1", IE_Pressed, this, &AMarblePlayerController::ViewMarble1);
        InputComponent->BindAction("ViewMarble2", IE_Pressed, this, &AMarblePlayerController::ViewMarble2);
        InputComponent->BindAction("ViewMarble3", IE_Pressed, this, &AMarblePlayerController::ViewMarble3);
        InputComponent->BindAction("ViewMarble4", IE_Pressed, this, &AMarblePlayerController::ViewMarble4);
        InputComponent->BindAction("ViewMarble5", IE_Pressed, this, &AMarblePlayerController::ViewMarble5);

        InputComponent->BindAction("NextMarble", IE_Pressed, this, &AMarblePlayerController::CycleToNextMarble);
    }
}

void AMarblePlayerController::CycleToNextMarble()
{
    if (!bRaceIsActive) return;
    
    TArray<AActor*> FoundMarbles;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("RaceMarble"), FoundMarbles);

    if (FoundMarbles.Num() == 0) return;
    
    CurrentViewIndex = (CurrentViewIndex + 1) % FoundMarbles.Num(); 
    FocusOnMarble(CurrentViewIndex);
}

void AMarblePlayerController::ViewMarble1() { FocusOnMarble(0); CurrentViewIndex = 0; }
void AMarblePlayerController::ViewMarble2() { FocusOnMarble(1); CurrentViewIndex = 1; }
void AMarblePlayerController::ViewMarble3() { FocusOnMarble(2); CurrentViewIndex = 2; }
void AMarblePlayerController::ViewMarble4() { FocusOnMarble(3); CurrentViewIndex = 3; }
void AMarblePlayerController::ViewMarble5() { FocusOnMarble(4); CurrentViewIndex = 4; }

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

void AMarblePlayerController::SwitchToAnalysView()
{
    AActor* AnalyseCam = FindCameraByTag(FName("AnalyseCam"));
    if (AnalyseCam)
    {
        SetViewTargetWithBlend(AnalyseCam, 2.0f, EViewTargetBlendFunction::VTBlend_EaseInOut, 2.0f);
    }
    bRaceIsActive = false;
}

void AMarblePlayerController::SwitchToConfigView()
{
    AActor* ConfigCam = FindCameraByTag(FName("ConfigCam"));
    if (ConfigCam)
    {
        SetViewTargetWithBlend(ConfigCam, 2.0f, EViewTargetBlendFunction::VTBlend_EaseInOut, 2.0f);
    }
    bRaceIsActive = false;
}

void AMarblePlayerController::SwitchToConfigViewFromRace(FVector LastCameraLocation, FRotator LastCameraRotation)
{
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    
    GhostCameraActor = GetWorld()->SpawnActor<ACameraActor>(LastCameraLocation, LastCameraRotation, SpawnParams);
    AActor* ConfigCam = FindCameraByTag(FName("ConfigCam"));
    
    if (GhostCameraActor)
    {
        SetViewTarget(GhostCameraActor);
    }
    
    if (ConfigCam) 
    {
        SetViewTargetWithBlend(ConfigCam, 1.5f, EViewTargetBlendFunction::VTBlend_EaseInOut, 2.0f);
    }
    
    GetWorld()->GetTimerManager().SetTimer(TimerHandle_CleanupGhost, this, &AMarblePlayerController::CleanupGhostCamera, 2.0f, false);
    
    bRaceIsActive = false;
}

void AMarblePlayerController::CleanupGhostCamera()
{
    if (GhostCameraActor)
    {
        GhostCameraActor->Destroy();
        GhostCameraActor = nullptr;
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
        CurrentViewedMarble = TargetMarble;
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