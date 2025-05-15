// EnemyCharacter1.cpp

#include "EnemyCharacter1.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "EnemyProjectile.h"
#include "EndlessShooterGameMode.h"

AEnemyCharacter1::AEnemyCharacter1()
{
    PrimaryActorTick.bCanEverTick = true;
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    AIControllerClass = AAIController::StaticClass();
}

void AEnemyCharacter1::BeginPlay()
{
    Super::BeginPlay();

    if (auto GM = Cast<AEndlessShooterGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        float Scale = GM->EnemyHealthScale;
        CurrentHealth = MaxHealth * Scale;
    }
    AICon = Cast<AAIController>(GetController());

    // kick off roaming
    GetWorldTimerManager()
        .SetTimer(RoamTimerHandle, this, &AEnemyCharacter1::Roam, RoamInterval, true, 0.f);
}

void AEnemyCharacter1::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (!AICon) return;

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
    if (!PlayerPawn) return;

    float Dist = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());
    bool bCanSee = Dist <= DetectionRadius && AICon->LineOfSightTo(PlayerPawn);

    if (bCanSee && !bChasing)
        StartChase();
    else if (!bCanSee && bChasing)
        StopChase();

    if (bChasing)
    {
        float Now = GetWorld()->GetTimeSeconds();

        if (bIsMelee)
        {
            // melee attack
            if (Dist <= MeleeRange && Now - LastMeleeTime >= MeleeAttackInterval)
            {
                UGameplayStatics::ApplyDamage(PlayerPawn, MeleeDamage, AICon, this, nullptr);

                if (GEngine)
                    GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red,
                        FString::Printf(TEXT("Enemy melee hits %.1f"), MeleeDamage));

                LastMeleeTime = Now;
            }
        }
        else
        {
            // ranged attack
            if (RangedProjectileClass && Now - LastRangedTime >= RangedAttackInterval)
            {
                FVector MuzzleWorld = GetActorLocation()
                    + GetActorRotation().RotateVector(RangedMuzzleOffset);

                // aim at player
                FVector Dir = (PlayerPawn->GetActorLocation() - MuzzleWorld).GetSafeNormal();
                FRotator ProjRot = Dir.Rotation();

                FActorSpawnParameters P;
                P.Owner = this;

                if (auto Proj = GetWorld()->SpawnActor<AEnemyProjectile>(
                        RangedProjectileClass, MuzzleWorld, ProjRot, P))
                {
                    // debug
                    if (GEngine)
                        GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow,
                            TEXT("Enemy fires projectile!"));
                }

                LastRangedTime = Now;
            }
        }
    }
}

void AEnemyCharacter1::Roam()
{
    if (bChasing || !AICon) return;

    if (UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
    {
        FNavLocation RandPt;
        if (Nav->GetRandomPointInNavigableRadius(GetActorLocation(), RoamRadius, RandPt))
            AICon->MoveToLocation(RandPt.Location);
    }
}

void AEnemyCharacter1::StartChase()
{
    bChasing = true;
    GetWorldTimerManager().ClearTimer(RoamTimerHandle);

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
    if (PlayerPawn)
        AICon->MoveToActor(PlayerPawn, bIsMelee ? MeleeRange : 5.f);
}

void AEnemyCharacter1::StopChase()
{
    bChasing = false;
    AICon->StopMovement();
    GetWorldTimerManager()
        .SetTimer(RoamTimerHandle, this, &AEnemyCharacter1::Roam, RoamInterval, true, 0.f);
}

float AEnemyCharacter1::TakeDamage(float DamageAmount,
                                  const FDamageEvent& DamageEvent,
                                  AController* EventInstigator,
                                  AActor* DamageCauser)
{
    float Actual = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    if (Actual <= 0.f) return 0.f;

    CurrentHealth -= Actual;
    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Blue,
            FString::Printf(TEXT("Enemy HP: %.1f"), CurrentHealth));

    if (CurrentHealth <= 0.f)
    {
        if (auto GM = Cast<AEndlessShooterGameMode>(UGameplayStatics::GetGameMode(this)))
            GM->RegisterEnemyKill();

        Destroy();
    }
    return Actual;
}
