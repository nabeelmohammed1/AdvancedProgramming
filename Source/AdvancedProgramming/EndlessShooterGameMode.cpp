// EndlessShooterGameMode.cpp

#include "EndlessShooterGameMode.h"
#include "UpgradePromptWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

void AEndlessShooterGameMode::RegisterEnemyKill()
{
    // track toward upgrade prompt
    KillCount++;
    TryTriggerUpgrade();

    // track toward enemy health scaling
    HealthKillCount++;
    TryHealthIncrease();
}

void AEndlessShooterGameMode::TryTriggerUpgrade()
{
    if (!ActiveWidget && KillCount >= KillsForUpgrade)
    {
        ShowUpgradePrompt();

        // prepare next threshold
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
    if (ActiveWidget)
    {
        ActiveWidget->AddToViewport();
        if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
        {
            PC->bShowMouseCursor = true;
            PC->SetInputMode(FInputModeUIOnly().SetWidgetToFocus(ActiveWidget->TakeWidget()));
        }
        UGameplayStatics::SetGamePaused(this, true);
    }
}

void AEndlessShooterGameMode::NotifyUpgradeComplete()
{
    if (ActiveWidget)
    {
        ActiveWidget = nullptr;
        if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
        {
            PC->bShowMouseCursor = false;
            PC->SetInputMode(FInputModeGameOnly());
        }
        UGameplayStatics::SetGamePaused(this, false);
    }
}
