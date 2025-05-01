// Fill out your copyright notice in the Description page of Project Settings.
// EnemySpawner.cpp
#include "EnemySpawner.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "TimerManager.h"

AEnemySpawner::AEnemySpawner()
{
    PrimaryActorTick.bCanEverTick = false;

    // Create and expose a box you can resize/move in-editor or Blueprint
    SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
    SpawnVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    RootComponent = SpawnVolume;
}

void AEnemySpawner::BeginPlay()
{
    Super::BeginPlay();
    // Start recurring spawn
    GetWorld()->GetTimerManager()
        .SetTimer(TimerHandle_Spawn, this, &AEnemySpawner::SpawnEnemy, SpawnInterval, true);
}

int32 AEnemySpawner::GetCurrentSpawnIndex() const
{
    const float Now = GetWorld()->GetTimeSeconds();
    int32 Index = 0;
    for (int32 i = 0; i < SpawnInfos.Num(); ++i)
    {
        if (Now >= SpawnInfos[i].BeginTime)
            Index = i;
        else
            break;
    }
    return Index;
}

void AEnemySpawner::SpawnEnemy()
{
    const int32 Tier = GetCurrentSpawnIndex();
    if (!SpawnInfos.IsValidIndex(Tier) || !SpawnInfos[Tier].EnemyClass)
        return;

    // pick a random point inside the box
    const FVector Center = SpawnVolume->GetComponentLocation();
    const FVector Extents = SpawnVolume->GetScaledBoxExtent();
    const FVector SpawnLoc = UKismetMathLibrary::RandomPointInBoundingBox(Center, Extents);

    FActorSpawnParameters Params;
    GetWorld()->SpawnActor<AActor>(
        SpawnInfos[Tier].EnemyClass,
        SpawnLoc,
        FRotator::ZeroRotator,
        Params
    );
}
