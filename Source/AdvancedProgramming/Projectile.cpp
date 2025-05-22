// Projectile.cpp
#include "Projectile.h"
#include "EnemyCharacter1.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AProjectile::AProjectile()
{
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    CollisionComponent->InitSphereRadius(5.f);
    CollisionComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
    RootComponent = CollisionComponent;

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjMovement"));
    ProjectileMovement->UpdatedComponent = CollisionComponent;
    ProjectileMovement->InitialSpeed = 3000.f;
    ProjectileMovement->MaxSpeed = 3000.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;
    InitialLifeSpan = 3.f;
}

void AProjectile::BeginPlay()
{
    Super::BeginPlay();
    
    if (auto Mesh = FindComponentByClass<UStaticMeshComponent>())
        {
            // assume your mesh uses a material with a VectorParameter named "Tint"
            auto Dyn = UMaterialInstanceDynamic::Create(Mesh->GetMaterial(0), this);
            Mesh->SetMaterial(0, Dyn);
            Dyn->SetVectorParameterValue(TEXT("TintColor"), TintColor);
        }
    
    if (AActor* MyOwner = GetOwner())
        CollisionComponent->IgnoreActorWhenMoving(MyOwner, true);
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (!OtherActor || OtherActor == this || !OtherComp) return;

    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::White, FString::Printf(TEXT("Hit: %s"), *OtherActor->GetName()));

    if (AEnemyCharacter1* Enemy = Cast<AEnemyCharacter1>(OtherActor))
    {
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("Enemy %s hit!"), *Enemy->GetName()));
    }

    UGameplayStatics::ApplyPointDamage(OtherActor, Damage, GetVelocity().GetSafeNormal(), Hit,
        GetInstigatorController(), this, nullptr);

    if (OtherComp->IsSimulatingPhysics())
        OtherComp->AddImpulseAtLocation(GetVelocity()*100.f, Hit.ImpactPoint);

    Destroy();
}
