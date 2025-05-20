#include "EnemySpawner.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "EndlessShooterGameMode.h"
#include "EnemyCharacter1.h"

AEnemySpawner::AEnemySpawner()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AEnemySpawner::BeginPlay()
{
    Super::BeginPlay();

    // Register ourselves with the GameMode
    if (AEndlessShooterGameMode* GM = Cast<AEndlessShooterGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        GM->Spawners.Add(this);
    }

    // Kick off regular spawn timer
    GetWorldTimerManager().SetTimer(
        SpawnTimerHandle,
        this,
        &AEnemySpawner::SpawnRegular,
        SpawnInterval,
        true // loop
    );
}

void AEnemySpawner::SpawnRegular()
{
    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("Spawner → Regular"));

    SpawnByClass(RegularClass);
}

void AEnemySpawner::SpawnElite()
{
    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Cyan, TEXT("Spawner → Elite"));

    SpawnByClass(EliteClass);
}

void AEnemySpawner::SpawnBoss()
{
    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Spawner → Boss"));

    SpawnByClass(BossClass);
}

void AEnemySpawner::SpawnByClass(TSubclassOf<AEnemyCharacter1> ToSpawn)
{
    if (!ToSpawn) return;

    FActorSpawnParameters Params;
    GetWorld()->SpawnActor<AEnemyCharacter1>(
        ToSpawn,
        GetActorLocation(),
        GetActorRotation(),
        Params
    );
}
