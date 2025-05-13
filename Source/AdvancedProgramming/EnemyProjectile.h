// EnemyProjectile.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "EnemyProjectile.generated.h"

UCLASS()
class ADVANCEDPROGRAMMING_API AEnemyProjectile : public AActor
{
    GENERATED_BODY()

public:
    AEnemyProjectile();

protected:
    /** Collision component for detecting hits */
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
    USphereComponent* CollisionComp;

    /** Movement component to drive the projectile */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    UProjectileMovementComponent* ProjectileMovement;

    /** How much damage this projectile deals on hit */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float Damage = 20.f;

    virtual void BeginPlay() override;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
               UPrimitiveComponent* OtherComp, FVector NormalImpulse,
               const FHitResult& Hit);
};
