#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

UCLASS()
class ADVANCEDPROGRAMMING_API AEnemySpawner : public AActor
{
    GENERATED_BODY()

public:
    AEnemySpawner();

    /** Your base grunt class */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
    TSubclassOf<class AEnemyCharacter1> RegularClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
    TSubclassOf<class AEnemyCharacter1> EliteClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
    TSubclassOf<class AEnemyCharacter1> BossClass;

    /** Spawns one regular enemy */
    UFUNCTION()
    void SpawnRegular();

    /** Spawns one elite enemy */
    UFUNCTION()
    void SpawnElite();

    /** Spawns one boss enemy */
    UFUNCTION()
    void SpawnBoss();

protected:
    virtual void BeginPlay() override;

    /** How often to fire off regular spawns */
    UPROPERTY(EditDefaultsOnly, Category="Spawning")
    float SpawnInterval = 2.f;

private:
    void SpawnByClass(TSubclassOf<class AEnemyCharacter1> ToSpawn);

    FTimerHandle SpawnTimerHandle;
};
