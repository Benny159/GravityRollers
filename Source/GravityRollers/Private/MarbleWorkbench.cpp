#include "MarbleWorkbench.h"

AMarbleWorkbench::AMarbleWorkbench()
{
    PrimaryActorTick.bCanEverTick = false;
    
    BoardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoardMesh"));
    RootComponent = BoardMesh;

    SlotsRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SlotsRoot"));
    SlotsRoot->SetupAttachment(BoardMesh);
    
    for (int32 i = 0; i < 5; i++)
    {
        FString ArrowName = FString::Printf(TEXT("SlotArrow_%d"), i + 1);
        UArrowComponent* Arrow = CreateDefaultSubobject<UArrowComponent>(*ArrowName);
        Arrow->SetupAttachment(SlotsRoot);
        
        Arrow->SetRelativeLocation(FVector(0, i * 150.0f - 300.0f, 50.0f)); 
        
        Arrow->ArrowSize = 1.0f;
        Arrow->bTreatAsASprite = true;

        SlotArrows.Add(Arrow);
    }
}

void AMarbleWorkbench::BeginPlay()
{
    Super::BeginPlay();

    if (!MarbleClass) 
    {
        UE_LOG(LogTemp, Error, TEXT("Workbench: Keine MarbleClass gesetzt!"));
        return;
    }
    
    for (int32 i = 0; i < 5; i++)
    {
        if (SlotArrows.IsValidIndex(i) && SlotArrows[i])
        {
            FTransform SpawnTransform = SlotArrows[i]->GetComponentTransform();
            
            AMarble* NewMarble = GetWorld()->SpawnActor<AMarble>(MarbleClass, SpawnTransform);
            
            if (NewMarble)
            {
                NewMarble->SetFrozen(true);       
                NewMarble->StartingLaneIndex = i; 
                
                NewMarble->AttachToComponent(SlotArrows[i], FAttachmentTransformRules::KeepWorldTransform);
                
                if(NewMarble->MarbleMesh)
                {
                    NewMarble->MarbleMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
                    NewMarble->MarbleMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
                }

                ConfigMarbles.Add(NewMarble);
            }
        }
    }
}

void AMarbleWorkbench::SwapLaneAssignments(AMarble* TargetMarble, int32 DesiredLaneIndex)
{
    if (!TargetMarble || DesiredLaneIndex < 0 || DesiredLaneIndex > 4) return;
    
    int32 OldLaneIndex = TargetMarble->StartingLaneIndex;
    
    if (OldLaneIndex == DesiredLaneIndex) return;
    AMarble* ConflictingMarble = nullptr;

    for (AMarble* M : ConfigMarbles)
    {
        if (M && M != TargetMarble && M->StartingLaneIndex == DesiredLaneIndex)
        {
            ConflictingMarble = M;
            break;
        }
    }
    
    TargetMarble->StartingLaneIndex = DesiredLaneIndex;
    
    if (ConflictingMarble)
    {
        ConflictingMarble->StartingLaneIndex = OldLaneIndex;
        UE_LOG(LogTemp, Log, TEXT("Bahn getauscht: %s ist jetzt Bahn %d, %s ist jetzt Bahn %d"), 
            *TargetMarble->GetName(), DesiredLaneIndex + 1, 
            *ConflictingMarble->GetName(), OldLaneIndex + 1);
    }
}

TArray<FMarbleData> AMarbleWorkbench::GetAllMarbleData()
{
    TArray<FMarbleData> AllData;
    for (AMarble* Marble : ConfigMarbles)
    {
        if (Marble) AllData.Add(Marble->GetMarbleData());
        else AllData.Add(FMarbleData());
    }
    return AllData;
}