// EnemyCharacter1.cpp
#include "EnemyCharacter1.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "PlayerCharacter.h"
#include "NavigationSystem.h"
#include "TimerManager.h"
#include "Engine/Engine.h"

AEnemyCharacter1::AEnemyCharacter1()
{
    PrimaryActorTick.bCanEverTick = true;
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    AIControllerClass = AAIController::StaticClass();
}

void AEnemyCharacter1::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = MaxHealth;
    AICon = Cast<AAIController>(GetController());

    GetWorldTimerManager().SetTimer(RoamTimerHandle, this, &AEnemyCharacter1::Roam, RoamInterval, true, 0.f);
}

void AEnemyCharacter1::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (!AICon) return;

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
    if (!PlayerPawn) return;

    const float Dist = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());
    const bool bCanSee = Dist <= DetectionRadius && AICon->LineOfSightTo(PlayerPawn);

    if (bCanSee && !bChasing)
        StartChase();
    else if (!bCanSee && bChasing)
        StopChase();
}

void AEnemyCharacter1::Roam()
{
    if (bChasing || !AICon) return;

    FNavLocation Dest;
    if (UNavigationSystemV1* Nav = UNavigationSystemV1::GetCurrent(GetWorld()))
    {
        if (Nav->GetRandomPointInNavigableRadius(GetActorLocation(), RoamRadius, Dest))
            AICon->MoveToLocation(Dest.Location);
    }
}

void AEnemyCharacter1::StartChase()
{
    bChasing = true;
    GetWorldTimerManager().ClearTimer(RoamTimerHandle);
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
    if (PlayerPawn)
        AICon->MoveToActor(PlayerPawn, 5.f);
}

void AEnemyCharacter1::StopChase()
{
    bChasing = false;
    AICon->StopMovement();
    GetWorldTimerManager().SetTimer(RoamTimerHandle, this, &AEnemyCharacter1::Roam, RoamInterval, true, 0.f);
}

float AEnemyCharacter1::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    if (ActualDamage <= 0.f) return 0.f;

    CurrentHealth -= ActualDamage;
    // Print new health
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Blue,
            FString::Printf(TEXT("Enemy health: %.1f"), CurrentHealth));
    }

    if (CurrentHealth <= 0.f)
    {
        // notify GM that we died
        if (auto GM = Cast<AEndlessShooterGameMode>(UGameplayStatics::GetGameMode(this)))
        {
            GM->RegisterEnemyKill();
        }

        Destroy();
    }
    return ActualDamage;
}
