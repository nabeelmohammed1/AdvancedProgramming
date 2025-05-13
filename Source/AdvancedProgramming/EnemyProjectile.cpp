// EnemyProjectile.cpp
#include "EnemyProjectile.h"
#include "Kismet/GameplayStatics.h"

AEnemyProjectile::AEnemyProjectile()
{
    PrimaryActorTick.bCanEverTick = false;

    // Create and configure collision
    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    CollisionComp->InitSphereRadius(15.f);
    CollisionComp->SetCollisionProfileName(TEXT("Projectile"));
    CollisionComp->OnComponentHit.AddDynamic(this, &AEnemyProjectile::OnHit);
    RootComponent = CollisionComp;

    // Create movement component
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 800.f;
    ProjectileMovement->MaxSpeed = 800.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;

    InitialLifeSpan = 5.0f;
}

void AEnemyProjectile::BeginPlay()
{
    Super::BeginPlay();
}

void AEnemyProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
                             UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                             const FHitResult& Hit)
{
    if (OtherActor && OtherActor != this)
    {
        // Apply damage
        UGameplayStatics::ApplyDamage(OtherActor, Damage, GetInstigatorController(), this, nullptr);
    }
    Destroy();
}
