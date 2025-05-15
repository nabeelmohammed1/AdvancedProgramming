#include "EnemySpawner.h"
#include "Engine/World.h"
#include "TimerManager.h"

AEnemySpawner::AEnemySpawner()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AEnemySpawner::BeginPlay()
{
    Super::BeginPlay();

    SpawnTimers.SetNum(SpawnList.Num());

    for (int32 i = 0; i < SpawnList.Num(); ++i)
    {
        if (SpawnList[i].EnemyClass && SpawnList[i].SpawnInterval > 0)
        {
            GetWorldTimerManager().SetTimer(
                SpawnTimers[i],
                FTimerDelegate::CreateUObject(this, &AEnemySpawner::SpawnEnemy, i),
                SpawnList[i].SpawnInterval,
                true
            );
        }
    }
}

void AEnemySpawner::SpawnEnemy(int32 Index)
{
    if (!SpawnList.IsValidIndex(Index))
        return;

    FSpawnInfo& Info = SpawnList[Index];

    // if it's marked as one-shot and we've already done it, clear timer and bail out
    if (Info.bSpawnOnce && Info.bHasSpawned)
    {
        GetWorldTimerManager().ClearTimer(SpawnTimers[Index]);
        return;
    }

    FActorSpawnParameters Params;
    Params.Owner = this;

    FVector Loc = GetActorLocation();
    FRotator Rot = GetActorRotation();

    AActor* Spawned = GetWorld()->SpawnActor<AActor>(Info.EnemyClass, Loc, Rot, Params);
    if (Spawned)
    {
        Info.bHasSpawned = true;
        if (Info.bSpawnOnce)
        {
            GetWorldTimerManager().ClearTimer(SpawnTimers[Index]);
        }
    }
}
