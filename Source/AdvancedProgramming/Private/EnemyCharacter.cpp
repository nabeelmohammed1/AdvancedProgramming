// EnemyCharacter.cpp
#include "EnemyCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "TimerManager.h"

AEnemyCharacter::AEnemyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    AIControllerClass = AAIController::StaticClass();
}

void AEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

    AICon = Cast<AAIController>(GetController());
    PlayerPawn = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

    // Start roaming
    GetWorldTimerManager().SetTimer(RoamTimerHandle, this, &AEnemyCharacter::Roam, RoamInterval, true, 0.f);
}

void AEnemyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!PlayerPawn || !AICon) return;

    const float Dist = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());
    const bool bCanSee = Dist <= DetectionRadius &&
        AICon->LineOfSightTo(PlayerPawn);

    if (bCanSee && !bChasing)
    {
        StartChase();
    }
    else if (!bCanSee && bChasing)
    {
        StopChase();
    }
}

void AEnemyCharacter::Roam()
{
    if (bChasing || !AICon) return;

    FVector Origin = GetActorLocation();
    FNavLocation Dest;
    if (UNavigationSystemV1* Nav = UNavigationSystemV1::GetCurrent(GetWorld()))
    {
        if (Nav->GetRandomPointInNavigableRadius(Origin, RoamRadius, Dest))
        {
            AICon->MoveToLocation(Dest.Location);
        }
    }
}

void AEnemyCharacter::StartChase()
{
    bChasing = true;
    GetWorldTimerManager().ClearTimer(RoamTimerHandle);
    AICon->MoveToActor(PlayerPawn, 5.f);
}

void AEnemyCharacter::StopChase()
{
    bChasing = false;
    AICon->StopMovement();
    GetWorldTimerManager().SetTimer(RoamTimerHandle, this, &AEnemyCharacter::Roam, RoamInterval, true, 0.f);
}
