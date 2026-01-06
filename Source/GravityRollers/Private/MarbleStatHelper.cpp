#include "MarbleStatHelper.h"
#include "DataTrackerPlugin/DataTracker.h"

void UMarbleStatHelper::RecordStatPoint(FString DataSetName, float Time, float Value)
{
	UDataSet* Set = ADataTracker::GetDataSet(DataSetName);
    
	if (Set)
	{
		Set->Update(Time, Value, false, false, false, false, true, false);
	}
}

UDataSet* UMarbleStatHelper::GetDataSetByName(FString DataSetName)
{
	return ADataTracker::GetDataSet(DataSetName);
}

TArray<UDataSet*> UMarbleStatHelper::GetAllDataSets()
{
	return ADataTracker::GetAllDataSets();
}

TArray<FVector2D> UMarbleStatHelper::ConvertDataSetToGraphPoints(UDataSet* DataSet)
{
	TArray<FVector2D> Points;
	if (!DataSet) return Points;
	
	const TArray<float>& Times = DataSet->SampleSet.RecordingTime;
	const TArray<float>& Values = DataSet->SampleSet.Values;

	int32 Count = FMath::Min(Times.Num(), Values.Num());
	Points.Reserve(Count);

	for (int32 i = 0; i < Count; i++)
	{
		Points.Add(FVector2D(Times[i], Values[i]));
	}

	return Points;
}
void UMarbleStatHelper::ClearAllStatistics()
{
	if (ADataTracker::GDataTracker.IsValid())
	{
		ADataTracker::ClearAllDataSets();
	}
}
