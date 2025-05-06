#include "EnemyCharacter1.h"
#include "PlayerCharacter.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AEnemyCharacter1::AEnemyCharacter1()
{
    PrimaryActorTick.bCanEverTick = true;
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    AIControllerClass = AAIController::StaticClass();
}

void AEnemyCharacter1::BeginPlay()
{
    Super::BeginPlay();

    AICon = Cast<AAIController>(GetController());
    PlayerPawn = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

    // Start roaming timer
    GetWorldTimerManager().SetTimer(RoamTimerHandle, this, &AEnemyCharacter1::Roam, RoamInterval, true, 0.f);
}

void AEnemyCharacter1::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!PlayerPawn || !AICon) return;

    float Dist = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());
    bool bCanSee = (Dist <= DetectionRadius) && AICon->LineOfSightTo(PlayerPawn);

    if (bCanSee && !bChasing)
    {
        StartChase();
    }
    else if (!bCanSee && bChasing)
    {
        StopChase();
    }
}

void AEnemyCharacter1::Roam()
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

void AEnemyCharacter1::StartChase()
{
    bChasing = true;
    GetWorldTimerManager().ClearTimer(RoamTimerHandle);
    AICon->MoveToActor(PlayerPawn, 5.f);
}

void AEnemyCharacter1::StopChase()
{
    bChasing = false;
    AICon->StopMovement();
    GetWorldTimerManager().SetTimer(RoamTimerHandle, this, &AEnemyCharacter1::Roam, RoamInterval, true, 0.f);
}

