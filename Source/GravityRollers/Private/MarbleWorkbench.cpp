// Copyright (c) 2026 Gravity Rollers. All Rights Reserved.

#include "MarbleWorkbench.h"
#include "Marble.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"

AMarbleWorkbench::AMarbleWorkbench()
{
	PrimaryActorTick.bCanEverTick = false;
	
	BoardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoardMesh"));
	RootComponent = BoardMesh;

	SlotsRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SlotsRoot"));
	SlotsRoot->SetupAttachment(BoardMesh);
	
	DefaultMarbleColors.Add(FLinearColor(1.0f, 1.0f, 0.0f));
	DefaultMarbleColors.Add(FLinearColor(0.0f, 1.0f, 1.0f));
	DefaultMarbleColors.Add(FLinearColor(0.0f, 1.0f, 0.0f));
	DefaultMarbleColors.Add(FLinearColor(1.0f, 0.0f, 0.0f));
	DefaultMarbleColors.Add(FLinearColor(0.5f, 0.0f, 1.0f));

	CreateSlotArrows();
}

void AMarbleWorkbench::CreateSlotArrows()
{
	for (int32 i = 0; i < 5; i++)
	{
		const FString ArrowName = FString::Printf(TEXT("SlotArrow_%d"), i + 1);
		TObjectPtr<UArrowComponent> Arrow = CreateDefaultSubobject<UArrowComponent>(*ArrowName);
		
		if (Arrow)
		{
			Arrow->SetupAttachment(SlotsRoot);
			Arrow->SetRelativeLocation(FVector(0.f, (i * 150.0f) + -300.0f, 50.0f));
			Arrow->ArrowSize = 1.0f;
			Arrow->bTreatAsASprite = true;

			SlotArrows.Add(Arrow);
		}
	}
}

void AMarbleWorkbench::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnConfigMarbles();
}

void AMarbleWorkbench::SpawnConfigMarbles()
{
	if (!MarbleClass) 
	{
		return;
	}
	
	ConfigMarbles.Empty();

	for (int32 i = 0; i < SlotArrows.Num(); i++)
	{
		if (!SlotArrows[i])
		{
			continue;
		}

		const FTransform SpawnTransform = SlotArrows[i]->GetComponentTransform();
		AMarble* NewMarble = GetWorld()->SpawnActor<AMarble>(MarbleClass, SpawnTransform);
		
		if (NewMarble)
		{
			NewMarble->SetFrozen(true);       
			NewMarble->StartingLaneIndex = i;
			NewMarble->MarbleName = FString::Printf(TEXT("M_P%d"), i + 1);

			NewMarble->AttachToComponent(SlotArrows[i], FAttachmentTransformRules::KeepWorldTransform);
			
			if (UStaticMeshComponent* Mesh = NewMarble->GetMesh())
			{
				Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
			}

			NewMarble->Tags.Add(FName("ConfigMarble"));

			if (DefaultMarbleColors.IsValidIndex(i))
			{
				NewMarble->SetMarbleColor(DefaultMarbleColors[i]); 
			}

			ConfigMarbles.Add(NewMarble);
		}
	}
}

void AMarbleWorkbench::SwapLaneAssignments(AMarble* TargetMarble, int32 DesiredLaneIndex)
{
	const int32 OldLaneIndex = TargetMarble->StartingLaneIndex;
	const FString OldMarbleName = TargetMarble->MarbleName;
	if (!TargetMarble || DesiredLaneIndex < 0 || DesiredLaneIndex > 4 || OldLaneIndex == DesiredLaneIndex) 
	{
		return;
	}

	AMarble* ConflictingMarble = nullptr;

	for (AMarble* Marble : ConfigMarbles)
	{
		if (Marble && Marble != TargetMarble && Marble->StartingLaneIndex == DesiredLaneIndex)
		{
			ConflictingMarble = Marble;
			break;
		}
	}
	
	TargetMarble->StartingLaneIndex = DesiredLaneIndex;
	TargetMarble->MarbleName = FString::Printf(TEXT("M_P%d"), DesiredLaneIndex + 1);
	
	if (ConflictingMarble)
	{
		ConflictingMarble->StartingLaneIndex = OldLaneIndex;
		ConflictingMarble->MarbleName = OldMarbleName;
	}
}

TArray<FMarbleData> AMarbleWorkbench::GetAllMarbleData() const
{
	TArray<FMarbleData> AllData;
	AllData.Reserve(ConfigMarbles.Num());

	for (const AMarble* Marble : ConfigMarbles)
	{
		if (Marble)
		{
			AllData.Add(Marble->GetMarbleData());
		}
	}
	return AllData;
}