// Copyright (c) 2026 Gravity Rollers. All Rights Reserved.

#include "Marble.h"
#include "MarbleGameMode.h"
#include "MarblePlayerController.h"
#include "DataTrackerPlugin/DataSet/DataSet.h"
#include "DataTrackerPlugin/DataTracker.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

AMarble::AMarble()
{
	PrimaryActorTick.bCanEverTick = true;

	// Component Setup
	MarbleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MarbleMesh"));
	RootComponent = MarbleMesh;
	MarbleMesh->SetSimulatePhysics(true);
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); 
    
	CameraBoom->TargetArmLength = 2500.0f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, -60.f, 0.f));
	CameraBoom->bDoCollisionTest = false;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Default values
	bIsSelected = false;
	bHasFinished = false;
	bIsEliminated = false;
	StartingLaneIndex = -1;
	FinalRaceTime = 0.0f;
	FinalRaceSpeed = 0.0f;
	ScaleFactor = 100.0f;
	Size = 1.0f;
	Weight = 100.0f;
	SurfaceRoughness = 0.5f;
	MaterialDensity = 1.0f;
	MassDistribution = FVector::ZeroVector;
	Mass = 1.0f;
	Friction = 0.5f;
	Restitution = 0.5f;
	AngularDamping = 0.5f;
	FrictionCombineMode = EFrictionCombineMode::Min;
	RestitutionCombineMode = EFrictionCombineMode::Max;
	MarbleName = TEXT("Marble");
	MarbleColor = FLinearColor::White;
	
}

void AMarble::BeginPlay()
{
	Super::BeginPlay();
	InitialLocation = GetActorLocation();
	UpdatePhysicsProperties();
}

void AMarble::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CameraBoom)
	{
		CameraBoom->SetWorldLocation(GetActorLocation());
	}
	if (!MarbleMesh->IsSimulatingPhysics()) 
	{
		UpdateSelectionVisuals(DeltaTime);
	}
	
	UpdateRaceStats();
}

void AMarble::UpdateRaceStats()
{
	if (bHasFinished || bIsEliminated || ActorHasTag(FName("ConfigMarble")))
	{
		return;
	}

	AMarbleGameMode* MarbleGameMode = Cast<AMarbleGameMode>(GetWorld()->GetAuthGameMode());
	
	if (MarbleGameMode && MarbleGameMode->bRaceActive)
	{
		const float CurrentSpeed = GetVelocity().Size() / ScaleFactor;
		const float Time = GetWorld()->GetTimeSeconds() - MarbleGameMode->RaceStartTime;
		const FString StatName = TEXT("Speed@") + MarbleName;

		UDataSet* StatSet = ADataTracker::GetDataSet(StatName);
		if (StatSet)
		{
			StatSet->Update(Time, CurrentSpeed, true, true, true, false, false, false);
		}
	}
}

void AMarble::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);
	
	if ((MarbleMesh && MarbleMesh->IsSimulatingPhysics()) || !ActorHasTag(FName("ConfigMarble")))
	{
		return;
	}
	
	AMarblePlayerController* MarblePlayerController = Cast<AMarblePlayerController>(GetWorld()->GetFirstPlayerController());
	if (MarblePlayerController)
	{
		MarblePlayerController->SelectMarble(this);
	}
}

void AMarble::SetSelected(bool bSelected)
{
	bIsSelected = bSelected;
	if (bIsSelected)
	{
		InitialLocation = GetActorLocation();
	}
}

void AMarble::UpdateSelectionVisuals(float DeltaTime)
{
	const FVector TargetLoc = bIsSelected ? InitialLocation + FVector(0, 0, 150.0f) : InitialLocation;
	const FVector NewLoc = FMath::VInterpTo(GetActorLocation(), TargetLoc, DeltaTime, 10.0f);
	SetActorLocation(NewLoc);
}

FMarbleData AMarble::GetMarbleData() const
{
	FMarbleData Data;
	Data.MarbleName = MarbleName;
	Data.Size = Size;
	Data.Weight = Weight;
	Data.SurfaceRoughness = SurfaceRoughness;
	Data.MaterialDensity = MaterialDensity;
	Data.MassDistribution = MassDistribution;
	Data.Friction = Friction;
	Data.Restitution = Restitution;
	Data.AngularDamping = AngularDamping;
	Data.PreferredLaneIndex = StartingLaneIndex;
	Data.FinalRaceTime= FinalRaceTime;
	Data.FinalRaceSpeed = FinalRaceSpeed;
	Data.FinalRank = FinalRank;
	Data.MarbleColor = MarbleColor;

	return Data;
}

void AMarble::SetFrozen(bool bFrozen)
{
	if (MarbleMesh)
	{
		MarbleMesh->SetSimulatePhysics(!bFrozen);
	}
}

void AMarble::UpdatePhysicsProperties()
{
	if (!MarbleMesh)
	{
		return;
	}
	MarbleMesh->SetWorldScale3D(FVector(Size));

	Mass = Weight * MaterialDensity;
	MarbleMesh->SetMassOverrideInKg(NAME_None, Mass);

	MarbleMesh->BodyInstance.LinearDamping = SurfaceRoughness;
	MarbleMesh->BodyInstance.COMNudge = MassDistribution;
	MarbleMesh->BodyInstance.AngularDamping = AngularDamping;

	MarbleMesh->SetPhysMaterialOverride(CreatePhysicsMaterial());
}

UPhysicalMaterial* AMarble::CreatePhysicsMaterial()
{
	UPhysicalMaterial* PhysMat = NewObject<UPhysicalMaterial>(this);
	if (PhysMat)
	{
		PhysMat->Restitution = Restitution;
		PhysMat->Friction = Friction;
		PhysMat->FrictionCombineMode = FrictionCombineMode;
		PhysMat->RestitutionCombineMode = RestitutionCombineMode;
	}
	return PhysMat;
}

void AMarble::SetMarbleColor(FLinearColor NewColor)
{
	MarbleColor = NewColor;

	if (MarbleMesh)
	{
		UMaterialInstanceDynamic* DynMat = MarbleMesh->CreateAndSetMaterialInstanceDynamic(0);
		if (DynMat)
		{
			DynMat->SetVectorParameterValue(FName("BaseColor"), NewColor);
		}
	}
}

void AMarble::InitializeFromData(const FMarbleData& Data)
{
	MarbleName = Data.MarbleName;
	Size = Data.Size;
	Weight = Data.Weight;
	SurfaceRoughness = Data.SurfaceRoughness;
	MaterialDensity = Data.MaterialDensity;
	MassDistribution = Data.MassDistribution;
	Friction = Data.Friction;
	Restitution = Data.Restitution;
	AngularDamping = Data.AngularDamping;
	StartingLaneIndex = Data.PreferredLaneIndex;
	MarbleColor = Data.MarbleColor;

	SetMarbleColor(Data.MarbleColor);
	UpdatePhysicsProperties();
}

void AMarble::FinishRace(float TimeStamp, float FinishSpeed)
{
	if (bHasFinished || bIsEliminated)
	{
		return;
	}

	bHasFinished = true;
	FinalRaceTime = TimeStamp;
	FinalRaceSpeed = FinishSpeed / ScaleFactor;

	//Slow down Marble
	if (MarbleMesh)
	{
		MarbleMesh->SetLinearDamping(2.0f);
		MarbleMesh->SetAngularDamping(2.0f);
	}
}

void AMarble::Eliminate()
{
	if (bIsEliminated || bHasFinished)
	{
		return;
	}
	
	const float CrashSpeed = GetVelocity().Size() / ScaleFactor;
	float CrashTime = GetWorld()->GetTimeSeconds();

	if (AMarbleGameMode* MarbleGameMode = Cast<AMarbleGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		CrashTime = MarbleGameMode->GetCurrentRaceTime();
	}

	bIsEliminated = true;
	FinalRaceTime = CrashTime;
	FinalRaceSpeed = CrashSpeed;
}