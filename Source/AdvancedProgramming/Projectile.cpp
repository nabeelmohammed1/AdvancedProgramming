// Projectile.cpp
#include "Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/Engine.h"

AProjectile::AProjectile()
{
    // Use a sphere for collision
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    CollisionComponent->InitSphereRadius(5.f);
    CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
    CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
    RootComponent = CollisionComponent;

    // Projectile movement
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjMovement"));
    ProjectileMovement->UpdatedComponent = CollisionComponent;
    ProjectileMovement->InitialSpeed     = 3000.f;
    ProjectileMovement->MaxSpeed         = 3000.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce    = false;

    // Auto-destroy after 3 seconds
    InitialLifeSpan = 3.0f;
}

void AProjectile::BeginPlay()
{
    Super::BeginPlay();

    // Ignore collisions with the actor that fired this projectile
    if (AActor* MyOwner = GetOwner())
    {
        CollisionComponent->IgnoreActorWhenMoving(MyOwner, true);
    }
    // Also ignore all pawns
    CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
}

void AProjectile::OnHit(
    UPrimitiveComponent* HitComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    FVector NormalImpulse,
    const FHitResult& Hit
)
{
    // Debug print
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow,
            FString::Printf(TEXT("Projectile hit: %s"), *GetNameSafe(OtherActor)));
    }

    // Apply impulse if physics-enabled
    if (OtherComp && OtherComp->IsSimulatingPhysics())
    {
        OtherComp->AddImpulseAtLocation(GetVelocity() * 100.f, Hit.ImpactPoint);
    }

    Destroy();
}
