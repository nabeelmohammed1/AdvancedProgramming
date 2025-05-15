#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

USTRUCT(BlueprintType)
struct FSpawnInfo
{
    GENERATED_BODY()

    /** What enemy class to spawn (can be regular, elite, boss) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
    TSubclassOf<AActor> EnemyClass;

    /** Interval (seconds) between spawns */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
    float SpawnInterval = 5.f;

    /** If true, only spawn once (e.g. boss) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
    bool bSpawnOnce = false;

    /** Internal: have we already spawned it? */
    bool bHasSpawned = false;
};

UCLASS()
class ADVANCEDPROGRAMMING_API AEnemySpawner : public AActor
{
    GENERATED_BODY()

public:
    AEnemySpawner();

    /** Configure your waves, elites, bosses here */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
    TArray<FSpawnInfo> SpawnList;

protected:
    virtual void BeginPlay() override;

private:
    /** One timer per entry in SpawnList */
    TArray<FTimerHandle> SpawnTimers;

    /** Called by each timer to do a spawn */
    void SpawnEnemy(int32 Index);
};
