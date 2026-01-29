// Copyright (c) 2026 Gravity Rollers. All Rights Reserved.

#include "RaceTrack.h"
#include "Marble.h"
#include "MarbleGameMode.h"
#include "MarbleWorkbench.h"
#include "MarblePlayerController.h"
#include "Fan.h"
#include "Dog.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

ARaceTrack::ARaceTrack()
{
	PrimaryActorTick.bCanEverTick = false;

	// Component Setup
	TrackMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrackMesh"));
	RootComponent = TrackMesh;
	
	StartButton = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StartButton"));
	StartButton->SetupAttachment(RootComponent);
	
	EndTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("EndTrigger"));
	EndTrigger->SetupAttachment(RootComponent);
	EndTrigger->SetBoxExtent(FVector(100.f, 500.f, 100.f));
	EndTrigger->SetCollisionProfileName(TEXT("Trigger"));

	EliminationZone = CreateDefaultSubobject<UBoxComponent>(TEXT("EliminationZone"));
	EliminationZone->SetupAttachment(RootComponent);
	EliminationZone->SetBoxExtent(FVector(5000.f, 2000.f, 100.f)); 
	EliminationZone->SetRelativeLocation(FVector(0.f, 0.f, -1000.f)); 
	EliminationZone->SetCollisionProfileName(TEXT("Trigger"));
	
	StartPointsRoot = CreateDefaultSubobject<USceneComponent>(TEXT("StartPointsRoot"));
	StartPointsRoot->SetupAttachment(RootComponent);

	CreateStartPositions();

	// Default value
	bRaceStarted = false;
}

void ARaceTrack::CreateStartPositions()
{
	for (int32 i = 0; i < 5; i++)
	{
		const FString ArrowName = FString::Printf(TEXT("StartPosition_%d"), i + 1);
		TObjectPtr<UArrowComponent> Arrow = CreateDefaultSubobject<UArrowComponent>(*ArrowName);
		if (Arrow)
		{
			Arrow->SetupAttachment(StartPointsRoot);
			Arrow->SetRelativeLocation(FVector(0, i * 150.0f, 50.0f)); 
			StartPositions.Add(Arrow);
		}
	}
}

void ARaceTrack::BeginPlay()
{
	Super::BeginPlay();
	
	CacheGameReferences();

	if (EndTrigger)
	{
		EndTrigger->OnComponentBeginOverlap.AddDynamic(this, &ARaceTrack::OnEndOverlap);
	}

	if (EliminationZone)
	{
		EliminationZone->OnComponentBeginOverlap.AddDynamic(this, &ARaceTrack::OnEliminationZoneOverlap);
	}

	// Delay Spawn to be sure the Workbench has the MarbleDatas 
	FTimerHandle InitTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		InitTimerHandle, 
		this, 
		&ARaceTrack::InitialSpawnFromWorkbench, 
		0.2f,
		false
	);
}

void ARaceTrack::CacheGameReferences()
{
	MarbleGameMode = Cast<AMarbleGameMode>(UGameplayStatics::GetGameMode(this));
	MarblePlayerController = Cast<AMarblePlayerController>(GetWorld()->GetFirstPlayerController());

	if (!MarbleGameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("ARaceTrack: Failed to cache MarbleGameMode!"));
	}
	if (!MarblePlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("ARaceTrack: Failed to cache MarblePlayerController!"));
	}
}

void ARaceTrack::InitialSpawnFromWorkbench()
{
	AMarbleWorkbench* Workbench = Cast<AMarbleWorkbench>(
		UGameplayStatics::GetActorOfClass(this, AMarbleWorkbench::StaticClass())
	);

	if (Workbench)
	{
		const TArray<FMarbleData> InitialData = Workbench->GetAllMarbleData();
		SetupRaceFromData(InitialData);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("RaceTrack: Could not find Workbench for initial spawn!"));
	}
}

void ARaceTrack::SpawnMarble(TSubclassOf<AMarble> MarbleClass, int32 LaneIndex, const FMarbleData& InMarbleData)
{
	if (!MarbleClass || !StartPositions.IsValidIndex(LaneIndex) || !StartPositions[LaneIndex]) 
	{
		return;
	}

	const FTransform SpawnTransform = StartPositions[LaneIndex]->GetComponentTransform();
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AMarble* NewMarble = GetWorld()->SpawnActor<AMarble>(MarbleClass, SpawnTransform, SpawnParams);

	if (NewMarble)
	{
		NewMarble->InitializeFromData(InMarbleData);
		NewMarble->Tags.Add(FName("RaceMarble"));
		NewMarble->SetFrozen(true); 

		ActiveMarbles.Add(NewMarble);
	}
}

void ARaceTrack::SetupRaceFromData(const TArray<FMarbleData>& MarblesData)
{
	for (AMarble* Marble : ActiveMarbles)
	{
		if (IsValid(Marble))
		{
			Marble->Destroy();
		}
	}
	ActiveMarbles.Empty();
	
	for (const FMarbleData& Data : MarblesData)
	{
		const int32 TargetLane = Data.PreferredLaneIndex;
		
		if (StartPositions.IsValidIndex(TargetLane))
		{
			SpawnMarble(RaceMarbleClass, TargetLane, Data); 
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid LaneIndex in Data: %d"), TargetLane);
		}
	}
}

void ARaceTrack::StartRace()
{
	if (bRaceStarted)
	{
		return;
	}
	
	bRaceStarted = true;

	for (AMarble* Marble : ActiveMarbles)
	{
		if (Marble)
		{
			Marble->SetFrozen(false);
		}
	}
	
	if (MarblePlayerController)
	{
		MarblePlayerController->SetRaceState(true);
		MarblePlayerController->FocusOnMarble(0);
		
		if (MarblePlayerController->CurrentSelectedMarble)
		{
			MarblePlayerController->SelectMarble(MarblePlayerController->CurrentSelectedMarble);
		}
	}
	
	if (MarbleGameMode)
	{
		MarbleGameMode->StartRace(ActiveMarbles.Num());
	}

	AFan* Fan = Cast<AFan>(UGameplayStatics::GetActorOfClass(this, AFan::StaticClass()));
	if (Fan)
	{
		Fan->ActiveMarbles = ActiveMarbles;
	}

	ADog* Dog = Cast<ADog>(UGameplayStatics::GetActorOfClass(this, ADog::StaticClass()));
	if (Dog)
	{
		Dog->StartShockLoop();
	}
}

void ARaceTrack::ResetTrack()
{
	bRaceStarted = false;

	if (MarblePlayerController)
	{
		if (MarblePlayerController->IsRaceActive() && 
			MarblePlayerController->CurrentViewedMarble && 
			MarblePlayerController->CurrentViewedMarble->FollowCamera)
		{
			UCameraComponent* Cam = MarblePlayerController->CurrentViewedMarble->FollowCamera;
			MarblePlayerController->SwitchToConfigViewFromRace(Cam->GetComponentLocation(), Cam->GetComponentRotation());
		}
		else
		{
			MarblePlayerController->SwitchToConfigView();
		}
		MarblePlayerController->SetRaceState(false);
	}
	
	for (AMarble* Marble : ActiveMarbles)
	{
		if (IsValid(Marble))
		{
			Marble->Destroy();
		}
	}
	ActiveMarbles.Empty();

	if (MarbleGameMode)
	{
		MarbleGameMode->ResetRaceState();
	}

	AMarbleWorkbench* Workbench = Cast<AMarbleWorkbench>(
		UGameplayStatics::GetActorOfClass(this, AMarbleWorkbench::StaticClass())
	);
	
	if (Workbench)
	{
		SetupRaceFromData(Workbench->GetAllMarbleData());
	}
}

void ARaceTrack::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMarble* Marble = Cast<AMarble>(OtherActor);
	
	if (Marble && !Marble->bHasFinished && !Marble->bIsEliminated)
	{
		float FinishTime = 0.0f;
		
		if (MarbleGameMode)
		{
			FinishTime = MarbleGameMode->GetCurrentRaceTime();
		}
		else
		{
			FinishTime = GetWorld()->GetTimeSeconds();
		}

		const float FinishSpeed = Marble->GetVelocity().Size();
		
		Marble->FinishRace(FinishTime, FinishSpeed);
		
		if (MarbleGameMode)
		{
			Marble->FinalRank = MarbleGameMode->FinishedCount + 1;
			MarbleGameMode->RegisterMarble(Marble);
			MarbleGameMode->RegisterMarbleFinished();
		}

		if (MarblePlayerController && Marble == MarblePlayerController->CurrentViewedMarble)
		{
			MarblePlayerController->SwitchToNextActiveMarble();
		}
	}
}

void ARaceTrack::OnEliminationZoneOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMarble* Marble = Cast<AMarble>(OtherActor);
	if (Marble && !Marble->bIsEliminated && !Marble->bHasFinished)
	{
		Marble->Eliminate();
		
		if (MarbleGameMode)
		{
			Marble->FinalRank = 99;
			MarbleGameMode->RegisterMarble(Marble);
			MarbleGameMode->RegisterMarbleEliminated();
		}

		if (MarblePlayerController && Marble == MarblePlayerController->CurrentViewedMarble)
		{
			MarblePlayerController->SwitchToNextActiveMarble();
		}
	}
}