#include "EndlessShooterGameMode.h"
#include "UpgradePromptWidget.h"
#include "Kismet/GameplayStatics.h"

void AEndlessShooterGameMode::RegisterEnemyKill()
{
    KillCount++;
    TryTriggerUpgrade();
}

void AEndlessShooterGameMode::TryTriggerUpgrade()
{
    if (KillCount >= KillsForUpgrade && !ActiveWidget)
    {
        ShowUpgradePrompt();
        KillCount = 0;  // reset
    }
}

void AEndlessShooterGameMode::ShowUpgradePrompt()
{
    if (!UpgradeWidgetClass) return;

    ActiveWidget = CreateWidget<UUpgradePromptWidget>(GetWorld(), UpgradeWidgetClass);
    if (ActiveWidget)
    {
        ActiveWidget->AddToViewport();
        APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
        if (PC)
        {
            PC->bShowMouseCursor = true;
            PC->SetInputMode(FInputModeUIOnly());
        }
        // optionally pause the game:
        UGameplayStatics::SetGamePaused(this, true);
    }
}
