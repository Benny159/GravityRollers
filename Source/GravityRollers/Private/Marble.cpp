#include "Marble.h"
#include "MarbleGameMode.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Particles/ParticleSystemComponent.h"

AMarble::AMarble()
{
	PrimaryActorTick.bCanEverTick = true;
	
	MarbleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MarbleMesh"));
	RootComponent = MarbleMesh;
	MarbleMesh->SetSimulatePhysics(true);
	
	InnerCore = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InnerCore"));
	InnerCore->SetupAttachment(MarbleMesh);
	InnerCore->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	TrailEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TrailEffect"));
	TrailEffect->SetupAttachment(MarbleMesh);
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); 
    
	CameraBoom->TargetArmLength = 500.0f;
	CameraBoom->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
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
	UpdatePhysicsProperties();
}

void AMarble::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CameraBoom)
	{
		CameraBoom->SetWorldLocation(GetActorLocation());
	}
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

void AMarble::InitializeFromData(const FMarbleData& Data, int32 LaneIndex)
{
	Size = Data.Size;
	Weight = Data.Weight;
	SurfaceRoughness = Data.SurfaceRoughness;
	MaterialDensity = Data.MaterialDensity;
	MassDistribution = Data.MassDistribution;
	Friction = Data.Friction;
	Restitution = Data.Restitution;
	AngularDamping = Data.AngularDamping;
    
	StartingLaneIndex = LaneIndex;
	
	UpdatePhysicsProperties();

	UMaterialInstanceDynamic* DynMat = MarbleMesh->CreateAndSetMaterialInstanceDynamic(0);
	if(DynMat)
	{
		DynMat->SetVectorParameterValue(TEXT("BaseColor"), Data.MarbleColor);
	}
}

void AMarble::PassCheckpoint(int32 CheckpointIndex, float TimeStamp, float CurrentSpeed)
{
	if (bIsEliminated || bHasFinished) return;
	
	if (CheckpointTimes.Num() <= CheckpointIndex)
	{
		CheckpointTimes.SetNum(CheckpointIndex + 1);
		CheckpointSpeeds.SetNum(CheckpointIndex + 1);
	}
	
	CheckpointTimes[CheckpointIndex] = TimeStamp;
	CheckpointSpeeds[CheckpointIndex] = CurrentSpeed;
    
	UE_LOG(LogTemp, Log, TEXT("Murmel %s -> Checkpoint %d | Zeit: %.2fs | Speed: %.2f cm/s"), 
		*GetName(), CheckpointIndex, TimeStamp, CurrentSpeed);
}

void AMarble::FinishRace(float TimeStamp, float FinishSpeed)
{
	if (bHasFinished || bIsEliminated) return;

	bHasFinished = true;
	FinalRaceTime = TimeStamp;
	FinalRaceSpeed = FinishSpeed;
	
	MarbleMesh->SetLinearDamping(2.0f);
	MarbleMesh->SetAngularDamping(2.0f);

	UE_LOG(LogTemp, Warning, TEXT("ZIEL! Murmel %s | Zeit: %.2fs | Speed: %.2f cm/s"), 
		*GetName(), FinalRaceTime, FinalRaceSpeed);
}

void AMarble::Eliminate()
{
	if (bIsEliminated || bHasFinished) return;
	
	float CrashSpeed = GetVelocity().Size();
	float CrashTime = 0.0f;
	AMarbleGameMode* GM = Cast<AMarbleGameMode>(UGameplayStatics::GetGameMode(this));
	if (GM) 
	{
		CrashTime = GM->GetCurrentRaceTime();
		GM->RegisterMarbleEliminated();
	}
	else 
	{
		CrashTime = GetWorld()->GetTimeSeconds();
	}

	bIsEliminated = true;
	FinalRaceTime = CrashTime;
	FinalRaceSpeed = CrashSpeed;

	UE_LOG(LogTemp, Error, TEXT("ELIMINIERT! Murmel %s | Crash-Zeit: %.2fs | Crash-Speed: %.2f cm/s"), 
		*GetName(), FinalRaceTime, FinalRaceSpeed);
}