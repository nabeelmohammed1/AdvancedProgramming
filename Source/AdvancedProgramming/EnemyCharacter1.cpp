// === EnemyCharacter1.cpp ===
#include "EnemyCharacter1.h"
#include "Perception/PawnSensingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "EnemyProjectile.h"
#include "EndlessShooterGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h" // Optional: for debug drawing

AEnemyCharacter1::AEnemyCharacter1()
{
    PrimaryActorTick.bCanEverTick = true;

    PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
    PawnSensingComp->SightRadius = 1200.f;
    PawnSensingComp->SetPeripheralVisionAngle(60.f);
    PawnSensingComp->bSeePawns = true;
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
    if (!PlayerPawn)
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerPawn is null in BeginPlay"));
    }

    if (Variant == EEnemyVariant1::Elite || Variant == EEnemyVariant1::Boss)
    {
        bRanged = true;
    }

    Patrol();
}

void AEnemyCharacter1::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Melee range check
    if (!bRanged && PlayerPawn)
    {
        float Dist = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());
        if (Dist <= MeleeRange)
        {
            if (auto AIC = Cast<AAIController>(GetController()))
            {
                AIC->StopMovement();
            }
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
        {
            AIC->MoveToLocation(Dest.Location);
        }
    }

    GetWorldTimerManager().SetTimer(PatrolTimerHandle, this, &AEnemyCharacter1::Patrol, 3.f, false);
}

void AEnemyCharacter1::OnSeePawn(APawn* Pawn)
{
    if (!Pawn || Pawn != PlayerPawn) return;

    UE_LOG(LogTemp, Warning, TEXT("%s saw player! Ranged? %d"), *GetName(), bRanged);

    if (auto AIC = Cast<AAIController>(GetController()))
    {
        EPathFollowingRequestResult::Type Result = AIC->MoveToActor(PlayerPawn);
        UE_LOG(LogTemp, Warning, TEXT("MoveToActor result: %d"), (int32)Result);
    }

    Attack();
}

void AEnemyCharacter1::Attack()
{
    if (!PlayerPawn) return;

    if (bRanged && ProjectileClass)
    {
        FVector Muzzle = GetActorLocation() + FVector(0, 0, 50.f);
        FRotator Aim = (PlayerPawn->GetActorLocation() - Muzzle).Rotation();

        AEnemyProjectile* Proj = GetWorld()->SpawnActor<AEnemyProjectile>(ProjectileClass, Muzzle, Aim);
        if (Proj)
        {
            Proj->ProjectileMovement->Velocity = Aim.Vector() * ProjectileSpeed;
            UE_LOG(LogTemp, Warning, TEXT("%s fired projectile!"), *GetName());
        }

        GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AEnemyCharacter1::Attack, 2.f, false);
    }
    else if (!bRanged)
    {
        UGameplayStatics::ApplyDamage(PlayerPawn, MeleeDamage, GetController(), this, nullptr);
        UE_LOG(LogTemp, Warning, TEXT("%s performed melee attack!"), *GetName());
        GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AEnemyCharacter1::Attack, 1.5f, false);
    }
}
