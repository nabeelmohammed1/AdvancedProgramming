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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
    float Damage = 25.f;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleDefaultsOnly, Category="Projectile")
    USphereComponent* CollisionComponent;

    UPROPERTY(VisibleAnywhere, Category="Movement")
    UProjectileMovementComponent* ProjectileMovement;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
