#include "EndlessShooterGameMode.h"
#include "UpgradePromptWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "EngineUtils.h"
#include "EnemySpawner.h"

AEndlessShooterGameMode::AEndlessShooterGameMode()
{
    // Tune these in-editor or here:
    EliteEvery = 10;
    BossEvery  = 50;
    TotalKills = 0;
}

void AEndlessShooterGameMode::BeginPlay()
{
    Super::BeginPlay();

    // Gather every spawner in the world
    for (TActorIterator<AEnemySpawner> It(GetWorld()); It; ++It)
    {
        Spawners.Add(*It);
    }
}

void AEndlessShooterGameMode::RegisterEnemyKill(bool bIsBoss)
{
    TotalKills++;

    if (bIsBoss)
    {
        // Force an immediate upgrade prompt after boss defeat
        KillCount = KillsForUpgrade;
    }
    else
    {
        // Spawn elite every EliteEvery kills
        if (TotalKills % EliteEvery == 0)
        {
            for (auto* S : Spawners)
                S->SpawnElite();
        }
        // Spawn boss every BossEvery kills
        if (TotalKills % BossEvery == 0)
        {
            for (auto S : Spawners)
                S->SpawnBoss();
        }

        KillCount++;
        HealthKillCount++;
    }

    TryTriggerUpgrade();
    TryHealthIncrease();
}

void AEndlessShooterGameMode::TryTriggerUpgrade()
{
    if (!ActiveWidget && KillCount >= KillsForUpgrade)
    {
        ShowUpgradePrompt();
        // reset for next threshold
        KillCount = 0;
        KillsForUpgrade += UpgradeKillIncrement;
    }
}

void AEndlessShooterGameMode::TryHealthIncrease()
{
    if (HealthKillCount >= KillsPerHealthIncrease)
    {
        EnemyHealthScale *= HealthIncreaseScale;
        HealthKillCount = 0;
    }
}

void AEndlessShooterGameMode::ShowUpgradePrompt()
{
    if (!UpgradeWidgetClass) return;

    ActiveWidget = CreateWidget<UUpgradePromptWidget>(GetWorld(), UpgradeWidgetClass);
    if (!ActiveWidget) return;

    ActiveWidget->AddToViewport();

    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        PC->bShowMouseCursor = true;
        PC->SetInputMode(FInputModeUIOnly().SetWidgetToFocus(ActiveWidget->TakeWidget()));
    }

    UGameplayStatics::SetGamePaused(this, true);
}

void AEndlessShooterGameMode::NotifyUpgradeComplete()
{
    if (!ActiveWidget) return;

    ActiveWidget = nullptr;

    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        PC->bShowMouseCursor = false;
        PC->SetInputMode(FInputModeGameOnly());
    }

    UGameplayStatics::SetGamePaused(this, false);
}

