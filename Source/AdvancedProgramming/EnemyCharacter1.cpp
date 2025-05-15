// === EnemyCharacter1.cpp ===
#include "EnemyCharacter1.h"
#include "Perception/PawnSensingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "EnemyProjectile.h"
#include "EndlessShooterGameMode.h"

AEnemyCharacter1::AEnemyCharacter1()
{
    PrimaryActorTick.bCanEverTick = true;

    PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
    PawnSensingComp->SightRadius = 1200.f;
    PawnSensingComp->SetPeripheralVisionAngle(60.f);
    PawnSensingComp->OnSeePawn.AddDynamic(this, &AEnemyCharacter1::OnSeePawn);
}

void AEnemyCharacter1::BeginPlay()
{
    Super::BeginPlay();

    // Scale health by variant
    float Scale = (Variant == EEnemyVariant1::Elite) ? 1.5f : (Variant == EEnemyVariant1::Boss) ? 5.f : 1.f;
    if (auto GM = Cast<AEndlessShooterGameMode>(UGameplayStatics::GetGameMode(this)))
        Scale *= GM->EnemyHealthScale;

    CurrentHealth = BaseHealth * Scale;
    PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

    // start patrolling
    Patrol();
}

void AEnemyCharacter1::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // melee range check
    if (!bRanged && PlayerPawn)
    {
        float Dist = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());
        if (Dist <= MeleeRange)
        {
            if (auto AIC = Cast<AAIController>(GetController()))
                AIC->StopMovement();
            Attack();
        }
    }
}

float AEnemyCharacter1::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                  AController* EventInstigator, AActor* DamageCauser)
{
    CurrentHealth -= DamageAmount;
    if (CurrentHealth <= 0.f)
    {
        if (auto GM = Cast<AEndlessShooterGameMode>(UGameplayStatics::GetGameMode(this)))
            GM->RegisterEnemyKill();
        Destroy();
    }
    return DamageAmount;
}

void AEnemyCharacter1::Patrol()
{
    UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (!Nav) return;

    FNavLocation Dest;
    if (Nav->GetRandomPointInNavigableRadius(GetActorLocation(), 2000.f, Dest))
    {
        if (auto AIC = Cast<AAIController>(GetController()))
            AIC->MoveToLocation(Dest.Location);
    }
    GetWorldTimerManager().SetTimer(TimerHandle, this, &AEnemyCharacter1::Patrol, 3.f, false);
}

void AEnemyCharacter1::OnSeePawn(APawn* Pawn)
{
    if (!bRanged || Pawn != PlayerPawn) return;
    if (auto AIC = Cast<AAIController>(GetController()))
        AIC->MoveToActor(PlayerPawn);
    Attack();
}

void AEnemyCharacter1::Attack()
{
    if (bRanged && ProjectileClass && PlayerPawn)
    {
        FVector Muzzle = GetActorLocation() + FVector(0,0,50.f);
        FRotator Aim = (PlayerPawn->GetActorLocation() - Muzzle).Rotation();
        AEnemyProjectile* Proj = GetWorld()->SpawnActor<AEnemyProjectile>(ProjectileClass, Muzzle, Aim);
        if (Proj) Proj->ProjectileMovement->Velocity = Aim.Vector() * ProjectileSpeed;
        GetWorldTimerManager().SetTimer(TimerHandle, this, &AEnemyCharacter1::Attack, 2.f, false);
    }
    else if (!bRanged && PlayerPawn)
    {
        UGameplayStatics::ApplyDamage(PlayerPawn, MeleeDamage, GetController(), this, nullptr);
        GetWorldTimerManager().SetTimer(TimerHandle, this, &AEnemyCharacter1::Attack, 1.5f, false);
    }
}
