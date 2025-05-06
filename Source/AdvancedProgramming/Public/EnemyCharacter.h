// EnemyCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

class APlayerCharacter;
class AAIController;

UCLASS()
class ADVANCEDPROGRAMMING_API AEnemyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemyCharacter();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    /** Roam settings */
    UPROPERTY(EditAnywhere, Category = "AI")
    float RoamRadius = 800.f;

    UPROPERTY(EditAnywhere, Category = "AI")
    float RoamInterval = 5.f;

    /** Player detection */
    UPROPERTY(EditAnywhere, Category = "AI")
    float DetectionRadius = 1000.f;

private:
    /** Current player pawn */
    APlayerCharacter* PlayerPawn;

    /** AI controller reference */
    AAIController* AICon;

    /** Roam timer */
    FTimerHandle RoamTimerHandle;

    /** Are we currently chasing the player? */
    bool bChasing = false;

    /** Initiate a roam to a random point */
    void Roam();

    /** Begin chasing the player */
    void StartChase();

    /** Stop chasing and resume roaming */
    void StopChase();
};
