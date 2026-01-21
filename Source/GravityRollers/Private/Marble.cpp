#include "Marble.h"
#include "MarbleGameMode.h"
#include "DataTrackerPlugin/DataSet/DataSet.h"
#include "DataTrackerPlugin/DataTracker.h"
#include "MarblePlayerController.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Particles/ParticleSystemComponent.h"

AMarble::AMarble()
{
	PrimaryActorTick.bCanEverTick = true;
	bIsSelected = false;
	
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

	bHasFinished = false;
	bIsEliminated = false;
	StartingLaneIndex = -1;
	FinalRaceTime = 0.0f;
	FinalRaceSpeed = 0.0f;
	
}

void AMarble::BeginPlay()
{
	Super::BeginPlay();
	InitialLocation = GetActorLocation();
	UpdatePhysicsProperties();

	AMarbleGameMode* GM = Cast<AMarbleGameMode>(GetWorld()->GetAuthGameMode());
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
	AMarbleGameMode* GM = Cast<AMarbleGameMode>(GetWorld()->GetAuthGameMode());
	
	if (GM && GM->bRaceActive && !bHasFinished && !bIsEliminated && !ActorHasTag(FName("ConfigMarble")))
	{
		float RaceStartTime = GM->RaceStartTime;
		float CurrentSpeed = GetVelocity().Size() / ScaleFactor;
		float ActualRaceTime = GetWorld()->GetTimeSeconds();
		float Time = ActualRaceTime - RaceStartTime;
		
		FString StatName = "Speed@" + MarbleName;

		UDataSet* Set = ADataTracker::GetDataSet(StatName);
		
		if (Set)
		{
			Set->Update(Time, CurrentSpeed, true, true, true, false, false, false);
		}
	}
}

void AMarble::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);
	
	if (MarbleMesh->IsSimulatingPhysics()) return;

	if (!ActorHasTag(FName("ConfigMarble"))) 
	{
		return; 
	}
	
	AMarblePlayerController* PC = Cast<AMarblePlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC)
	{
		PC->SelectMarble(this);
	}
}

void AMarble::SetSelected(bool bSelected)
{
	bIsSelected = bSelected;
}

void AMarble::UpdateSelectionVisuals(float DeltaTime)
{
	FVector TargetLoc = InitialLocation;
	if (bIsSelected)
	{
		TargetLoc = InitialLocation + FVector(0, 0, 150.0f);
	}
	
	FVector NewLoc = FMath::VInterpTo(GetActorLocation(), TargetLoc, DeltaTime, 10.0f);
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
	MarbleMesh->SetWorldScale3D(FVector(Size));

	Mass = Weight * MaterialDensity;
	MarbleMesh->SetMassOverrideInKg(NAME_None, Mass);

	MarbleMesh->BodyInstance.LinearDamping = SurfaceRoughness;
	MarbleMesh->BodyInstance.COMNudge = MassDistribution;

	MarbleMesh->SetPhysMaterialOverride(CreatePhysicsMaterial());

	MarbleMesh->BodyInstance.AngularDamping = AngularDamping;

}

UPhysicalMaterial* AMarble::CreatePhysicsMaterial()
{
	UPhysicalMaterial* PhysMat = NewObject<UPhysicalMaterial>(this);

	PhysMat->Restitution = Restitution;
	PhysMat->Friction = Friction;

	PhysMat->FrictionCombineMode = FrictionCombineMode;
	PhysMat->RestitutionCombineMode = RestitutionCombineMode;

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
	if (bHasFinished || bIsEliminated) return;

	bHasFinished = true;
	FinalRaceTime = TimeStamp;
	FinalRaceSpeed = FinishSpeed / ScaleFactor;
	
	MarbleMesh->SetLinearDamping(2.0f);
	MarbleMesh->SetAngularDamping(2.0f);

	UE_LOG(LogTemp, Warning, TEXT("ZIEL! Murmel %s | Zeit: %.2fs | Speed: %.2f cm/s"), 
		*MarbleName, FinalRaceTime, FinalRaceSpeed);
}

void AMarble::Eliminate()
{
	if (bIsEliminated || bHasFinished) return;
	
	float CrashSpeed = GetVelocity().Size() / ScaleFactor;
	float CrashTime = 0.0f;
	AMarbleGameMode* GM = Cast<AMarbleGameMode>(UGameplayStatics::GetGameMode(this));
	if (GM) 
	{
		CrashTime = GM->GetCurrentRaceTime();
	}
	else 
	{
		CrashTime = GetWorld()->GetTimeSeconds();
	}

	bIsEliminated = true;
	FinalRaceTime = CrashTime;
	FinalRaceSpeed = CrashSpeed;

	UE_LOG(LogTemp, Error, TEXT("ELIMINIERT! Murmel %s | Crash-Zeit: %.2fs | Crash-Speed: %.2f cm/s"), 
		*MarbleName, FinalRaceTime, FinalRaceSpeed);
}