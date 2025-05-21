// === EnemyCharacter1.h ===
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter1.generated.h"

UENUM(BlueprintType)
enum class EEnemyVariant1 : uint8
{
    Normal UMETA(DisplayName="Normal"),
    Elite  UMETA(DisplayName="Elite"),
    Boss   UMETA(DisplayName="Boss")
};

UCLASS()
class ADVANCEDPROGRAMMING_API AEnemyCharacter1 : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemyCharacter1();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                             AController* EventInstigator, AActor* DamageCauser) override;

    // AI sensing
    UPROPERTY(VisibleAnywhere, Category="AI")
    class UPawnSensingComponent* PawnSensingComp;

    UFUNCTION()
    void OnSeePawn(APawn* Pawn);

    void Patrol();
    void Attack();

    // Variant scaling
    UPROPERTY(EditAnywhere, Category="Variant")
    EEnemyVariant1 Variant = EEnemyVariant1::Normal;

    UPROPERTY(EditAnywhere, Category="Variant", meta=(ClampMin=1))
    float BaseHealth = 50.f;
    float CurrentHealth;

    // Behavior
    UPROPERTY(EditAnywhere, Category="Combat")
    bool bRanged = false;

    // Melee
    UPROPERTY(EditAnywhere, Category="Combat", meta=(EditCondition="!bRanged"))
    float MeleeDamage = 10.f;
    UPROPERTY(EditAnywhere, Category="Combat", meta=(EditCondition="!bRanged"))
    float MeleeRange = 150.f;

    // Ranged
    UPROPERTY(EditAnywhere, Category="Combat", meta=(EditCondition="bRanged"))
    TSubclassOf<class AEnemyProjectile> ProjectileClass;
    UPROPERTY(EditAnywhere, Category="Combat", meta=(EditCondition="bRanged"))
    float ProjectileSpeed = 1200.f;
    
    //Timer
    FTimerHandle PatrolTimerHandle;
    FTimerHandle AttackTimerHandle;

private:
    APawn* PlayerPawn;
    FTimerHandle TimerHandle;
};
