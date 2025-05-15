// ======= EndlessShooterGameMode.h =======
#pragma once

#include "EndlessShooterGameMode.h"
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EndlessShooterGameMode.generated.h"

class UUpgradePromptWidget;

UCLASS()
class ADVANCEDPROGRAMMING_API AEndlessShooterGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    /** Called by enemies when they die; if bIsBoss==true, applies special boss logic */
    UFUNCTION()
    void RegisterEnemyKill(bool bIsBoss = false);

    /** Called by widget when upgrade selection completes */
    UFUNCTION()
    void NotifyUpgradeComplete();
    
    /** Global enemy health scale factor, bumps every KillsPerHealthIncrease */
    UPROPERTY(BlueprintReadOnly, Category="Upgrades|EnemyScaling")
    float EnemyHealthScale = 1.0f;

protected:
    /** How many kills before asking for the first upgrade */
    UPROPERTY(EditDefaultsOnly, Category="Upgrades|Thresholds")
    int32 KillsForUpgrade = 10;

    /** After each upgrade, add this many kills to the next threshold */
    UPROPERTY(EditDefaultsOnly, Category="Upgrades|Thresholds")
    int32 UpgradeKillIncrement = 5;

    /** How many kills before enemy health is bumped */
    UPROPERTY(EditDefaultsOnly, Category="Upgrades|EnemyScaling")
    int32 KillsPerHealthIncrease = 20;

    /** Multiply enemy health by this factor each time */
    UPROPERTY(EditDefaultsOnly, Category="Upgrades|EnemyScaling")
    float HealthIncreaseScale = 1.2f;

    /** The widget BP you made that has three buttons now */
    UPROPERTY(EditDefaultsOnly, Category="UI")
    TSubclassOf<UUpgradePromptWidget> UpgradeWidgetClass;

private:
    int32 KillCount = 0;
    int32 HealthKillCount = 0;
    UUpgradePromptWidget* ActiveWidget = nullptr;

    void TryTriggerUpgrade();
    void TryHealthIncrease();
    void ShowUpgradePrompt();
};

// ======= EndlessShooterGameMode.cpp =======
#include "EndlessShooterGameMode.h"
#include "UpgradePromptWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

void AEndlessShooterGameMode::RegisterEnemyKill(bool bIsBoss)
{
    if (bIsBoss)
    {
        // Special boss defeat: grant immediate upgrade prompt
        KillCount = KillsForUpgrade;
    }
    else
    {
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
