// EnemyCharacter1.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter1.generated.h"

class AAIController;
class AEnemyProjectile;
struct FTimerHandle;

UCLASS()
class ADVANCEDPROGRAMMING_API AEnemyCharacter1 : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemyCharacter1();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    /** AI roaming */
    UPROPERTY(EditAnywhere, Category="AI|Roam")
    float RoamRadius = 800.f;

    UPROPERTY(EditAnywhere, Category="AI|Roam")
    float RoamInterval = 5.f;

    /** How far the enemy will detect & chase the player */
    UPROPERTY(EditAnywhere, Category="AI|Detection")
    float DetectionRadius = 1000.f;

    /** Combat mode: if true, melee; if false, ranged */
    UPROPERTY(EditAnywhere, Category="Combat")
    bool bIsMelee = true;

    //────────── Melee settings ──────────//
    UPROPERTY(EditAnywhere, Category="Combat|Melee", meta=(EditCondition="bIsMelee"))
    float MeleeRange = 200.f;

    UPROPERTY(EditAnywhere, Category="Combat|Melee", meta=(EditCondition="bIsMelee"))
    float MeleeDamage = 20.f;

    UPROPERTY(EditAnywhere, Category="Combat|Melee", meta=(EditCondition="bIsMelee"))
    float MeleeAttackInterval = 1.f;

    //───────── Ranged settings ─────────//
    /** Must supply your own EnemyProjectile Blueprint or C++ subclass */
    UPROPERTY(EditAnywhere, Category="Combat|Ranged", meta=(EditCondition="!bIsMelee"))
    TSubclassOf<AEnemyProjectile> RangedProjectileClass;

    UPROPERTY(EditAnywhere, Category="Combat|Ranged", meta=(EditCondition="!bIsMelee"))
    float RangedAttackInterval = 2.f;

    /** Spawn offset for projectile (in local space) */
    UPROPERTY(EditAnywhere, Category="Combat|Ranged", meta=(EditCondition="!bIsMelee"))
    FVector RangedMuzzleOffset = FVector(100.f, 0.f, 50.f);

    /** Stats */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
    float MaxHealth = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stats")
    float CurrentHealth;

private:
    /** AI controller reference */
    AAIController* AICon;

    /** Roaming timer */
    FTimerHandle RoamTimerHandle;

    bool bChasing = false;

    /** Last attack timestamps */
    float LastMeleeTime = 0.f;
    float LastRangedTime = 0.f;

    void Roam();
    void StartChase();
    void StopChase();

    virtual float TakeDamage(float DamageAmount,
                             const FDamageEvent& DamageEvent,
                             AController* EventInstigator,
                             AActor* DamageCauser) override;
};
