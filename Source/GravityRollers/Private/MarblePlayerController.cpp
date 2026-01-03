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
    CurrentViewIndex = -1;
    CurrentSelectedMarble = nullptr;
}

void AMarblePlayerController::BeginPlay()
{
    Super::BeginPlay();
    SwitchToConfigView();
}

void AMarblePlayerController::SelectMarble(AMarble* NewMarble)
{
    if (CurrentSelectedMarble == NewMarble)
    {
        NewMarble->SetSelected(false);
        
        CurrentSelectedMarble = nullptr;

        UE_LOG(LogTemp, Log, TEXT("Murmel abgewählt: %s"), *NewMarble->GetName());
        
        // Optional: Hier Widget schließen!
        // if (MyConfigWidget) MyConfigWidget->RemoveFromParent();
        
        return;
    }
    if (CurrentSelectedMarble)
    {
        CurrentSelectedMarble->SetSelected(false);
    }
    
    CurrentSelectedMarble = NewMarble;
    
    if (CurrentSelectedMarble)
    {
        CurrentSelectedMarble->SetSelected(true);
        UE_LOG(LogTemp, Log, TEXT("Neue Murmel selektiert: %s"), *CurrentSelectedMarble->GetName());
        
        // HIER SPÄTER: Widget öffnen und aktualisieren!
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

void AMarblePlayerController::SwitchToConfigView()
{
    bRaceIsActive = false;
    AActor* ConfigCam = FindCameraByTag(FName("ConfigCam"));
    if (ConfigCam)
    {
        SetViewTargetWithBlend(ConfigCam, 2.0f, EViewTargetBlendFunction::VTBlend_EaseInOut, 2.0f);
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