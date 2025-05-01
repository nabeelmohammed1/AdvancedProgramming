// Fill out your copyright notice in the Description page of Project Settings.
// EnemySpawner.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

class UBoxComponent;

USTRUCT(BlueprintType)
struct FSpawnInfo
{
    GENERATED_BODY()

    /** Which enemy to spawn */
    UPROPERTY(EditAnywhere, Category="Spawning")
    TSubclassOf<AActor> EnemyClass;

    /** World time (s) at which this entry becomes active */
    UPROPERTY(EditAnywhere, Category="Spawning")
    float BeginTime = 0.f;
};

UCLASS()
class ADVANCEDPROGRAMMING_API AEnemySpawner : public AActor
{
    GENERATED_BODY()

public:
    AEnemySpawner();

protected:
    virtual void BeginPlay() override;

    /** Volume that defines where enemies may appear */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Spawning")
    UBoxComponent* SpawnVolume;

    /** Spawn tiers: ordered by BeginTime ascending */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawning")
    TArray<FSpawnInfo> SpawnInfos;

    /** Seconds between each spawn */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawning", meta=(ClampMin="0.1"))
    float SpawnInterval = 2.f;

private:
    FTimerHandle TimerHandle_Spawn;

    /** Called on each tick of the spawn timer */
    void SpawnEnemy();

    /** Find which tier is active based on world time */
    int32 GetCurrentSpawnIndex() const;
};
