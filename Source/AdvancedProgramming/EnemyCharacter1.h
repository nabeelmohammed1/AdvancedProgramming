// EnemyCharacter1.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter1.generated.h"

class AAIController;
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

    /** Roam settings */
    UPROPERTY(EditAnywhere, Category="AI")
    float RoamRadius = 800.f;

    UPROPERTY(EditAnywhere, Category="AI")
    float RoamInterval = 5.f;

    /** Detection radius for the player */
    UPROPERTY(EditAnywhere, Category="AI")
    float DetectionRadius = 1000.f;

    /** Enemy max health */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
    float MaxHealth = 100.f;

    /** Current health */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stats")
    float CurrentHealth;

private:
    /** Cached AI controller */
    AAIController* AICon;

    /** Timer handle for roaming */
    FTimerHandle RoamTimerHandle;

    bool bChasing = false;

    void Roam();
    void StartChase();
    void StopChase();

    /** Handle damage */
    virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};
