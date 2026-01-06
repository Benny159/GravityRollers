#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DataTrackerPlugin/DataSet/DataSet.h"
#include "MarbleStatHelper.generated.h"

UCLASS()
class GRAVITYROLLERS_API UMarbleStatHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Stats")
	static void RecordStatPoint(FString DataSetName, float Time, float Value);
	
	UFUNCTION(BlueprintCallable, Category = "Stats")
	static UDataSet* GetDataSetByName(FString DataSetName);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	static TArray<UDataSet*> GetAllDataSets();
	
	UFUNCTION(BlueprintCallable, Category = "Stats")
	static TArray<FVector2D> ConvertDataSetToGraphPoints(UDataSet* DataSet);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	static void ClearAllStatistics();
};