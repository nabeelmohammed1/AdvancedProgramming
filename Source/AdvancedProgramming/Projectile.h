// Projectile.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class ADVANCEDPROGRAMMING_API AProjectile : public AActor
{
    GENERATED_BODY()

public:
    AProjectile();

protected:
    virtual void BeginPlay() override;

    /** Sphere collision component */
    UPROPERTY(VisibleDefaultsOnly, Category="Projectile")
    USphereComponent* CollisionComponent;

    /** Projectile movement component */
    UPROPERTY(VisibleAnywhere, Category="Movement")
    UProjectileMovementComponent* ProjectileMovement;

    /** Handler for collision */
    UFUNCTION()
    void OnHit(
        UPrimitiveComponent* HitComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        FVector NormalImpulse,
        const FHitResult& Hit
    );
};
