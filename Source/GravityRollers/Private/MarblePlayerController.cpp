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
	bIsCameraSwitching = false;
    SwitchTime = 0.8f;
    VirtualBlendTime = 2.0f;
	CurrentViewIndex = -1;
	
	CurrentSelectedMarble = nullptr;
	CurrentViewedMarble = nullptr;
	GhostCameraActor = nullptr;
}

void AMarblePlayerController::BeginPlay()
{
    Super::BeginPlay();

    SwitchToConfigView();
    ADataTracker::ClearAllDataSets();
}

void AMarblePlayerController::SelectMarble(AMarble* NewMarble)
{
    if (CurrentSelectedMarble)
    {
        CurrentSelectedMarble->SetSelected(false);
    }

    OnHideMarbleUI();

    if (CurrentSelectedMarble == NewMarble)
    {
        CurrentSelectedMarble = nullptr;
    }
    else
    {
        CurrentSelectedMarble = NewMarble;
    }

    if (CurrentSelectedMarble)
    {
        CurrentSelectedMarble->SetSelected(true);
        OnShowMarbleUI(CurrentSelectedMarble);
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

        InputComponent->BindAction("NextMarble", IE_Pressed, this, &AMarblePlayerController::SwitchToNextActiveMarble);
    }
}

void AMarblePlayerController::UnlockCameraSwitch()
{
    bIsCameraSwitching = false;
}

void AMarblePlayerController::SwitchToNextActiveMarble()
{
    TArray<AActor*> FoundMarbles;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("RaceMarble"), FoundMarbles);
    
    if (FoundMarbles.IsEmpty())
	{
		return;
	}
    
    FoundMarbles.Sort([](const AActor& A, const AActor& B) {
        const AMarble* MarbleA = Cast<AMarble>(&A);
		const AMarble* MarbleB = Cast<AMarble>(&B);
		
		if (MarbleA && MarbleB)
		{
			return MarbleA->StartingLaneIndex < MarbleB->StartingLaneIndex;
		}
		return false;
    });
    
    int32 CurrentListIndex = -1;

    if (CurrentViewedMarble)
    {
        for (int32 Index = 0; Index < FoundMarbles.Num(); Index++)
        {
            if (FoundMarbles[Index] == CurrentViewedMarble)
            {
                CurrentListIndex = Index;
                break;
            }
        }
    }

    for (int32 Index = 1; Index < FoundMarbles.Num() + 1; Index++)
    {
        int32 CheckIndex = (CurrentListIndex + Index) % FoundMarbles.Num();
        AMarble* Candidate = Cast<AMarble>(FoundMarbles[CheckIndex]);

        if (Candidate && IsValid(Candidate))
        {
            if (!Candidate->bIsEliminated && !Candidate->bHasFinished)
            {   
                FocusOnMarble(Candidate->StartingLaneIndex);
                
                CurrentViewIndex = Candidate->StartingLaneIndex;
                CurrentViewedMarble = Candidate;
                return;
            }
        }
    }
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
        SetViewTargetWithBlend(RaceCam, SwitchTime, EViewTargetBlendFunction::VTBlend_EaseInOut, VirtualBlendTime);
    }
}

void AMarblePlayerController::SwitchToAnalysView()
{
    AActor* AnalyseCam = FindCameraByTag(FName("AnalyseCam"));
    if (AnalyseCam)
    {
        SetViewTargetWithBlend(AnalyseCam, SwitchTime, EViewTargetBlendFunction::VTBlend_EaseInOut, VirtualBlendTime);
    }
    bRaceIsActive = false;
}

void AMarblePlayerController::SwitchToConfigView()
{
    AActor* ConfigCam = FindCameraByTag(FName("ConfigCam"));
    if (ConfigCam)
    {
        SetViewTargetWithBlend(ConfigCam, SwitchTime, EViewTargetBlendFunction::VTBlend_EaseInOut, VirtualBlendTime);
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
        SetViewTargetWithBlend(ConfigCam, SwitchTime, EViewTargetBlendFunction::VTBlend_EaseInOut, VirtualBlendTime);
    }

    GetWorld()->GetTimerManager().SetTimer(TimerHandle_CleanupGhost, this, &AMarblePlayerController::CleanupGhostCamera, VirtualBlendTime, false);

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
    if (!bRaceIsActive || bIsCameraSwitching)
	{
		return;
	}
    
    TArray<AActor*> FoundMarbles;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("RaceMarble"), FoundMarbles);

    AMarble* TargetMarble = nullptr;
    
    for (AActor* Actor : FoundMarbles)
    {
        AMarble* Marble = Cast<AMarble>(Actor);
        if (Marble && Marble->StartingLaneIndex == MarbleIndex)
        {
            TargetMarble = Marble;
            break;
        }
    }
    
    if (TargetMarble)
    {
        bIsCameraSwitching = true;
        CurrentViewedMarble = TargetMarble;

        SetViewTargetWithBlend(TargetMarble, SwitchTime, EViewTargetBlendFunction::VTBlend_Cubic);
        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle_CameraCooldown, 
            this, 
            &AMarblePlayerController::UnlockCameraSwitch, 
            SwitchTime,
            false
        );
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
    
    return nullptr;
}
