// EnemyProjectile.cpp
#include "EnemyProjectile.h"
#include "Kismet/GameplayStatics.h"

AEnemyProjectile::AEnemyProjectile()
{
    PrimaryActorTick.bCanEverTick = false;

    // --- collision setup ---
    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    CollisionComp->InitSphereRadius(15.f);
    CollisionComp->SetCollisionProfileName(TEXT("Projectile"));
    CollisionComp->OnComponentHit.AddDynamic(this, &AEnemyProjectile::OnHit);
    RootComponent = CollisionComp;

    // we’ll also ignore the owner once it’s set
    CollisionComp->SetCanEverAffectNavigation(false);
    // (we’ll call IgnoreActorWhenMoving in BeginPlay)

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    // … your existing movement setup …

    InitialLifeSpan = 5.0f;
}

void AEnemyProjectile::BeginPlay()
{
    Super::BeginPlay();

    // if owner was set at spawn, ignore it
    if (GetOwner())
    {
        CollisionComp->IgnoreActorWhenMoving(GetOwner(), true);
    }
    // also ignore the instigator pawn (just in case)
    if (GetInstigator())
    {
        CollisionComp->IgnoreActorWhenMoving(GetInstigator(), true);
    }
}

void AEnemyProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
                             UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                             const FHitResult& Hit)
{
    // only damage things that aren’t me or my owner/instigator
    if (OtherActor &&
        OtherActor != this &&
        OtherActor != GetOwner() &&
        OtherActor != GetInstigator())
    {
        UGameplayStatics::ApplyDamage(
            OtherActor,
            Damage,
            GetInstigatorController(),
            this,
            nullptr
        );
    }

    Destroy();
}
