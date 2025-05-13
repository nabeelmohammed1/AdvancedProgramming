// EndlessShooterGameMode.h
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EndlessShooterGameMode.generated.h"

class UUpgradePromptWidget;

UCLASS()
class ADVANCEDPROGRAMMING_API AEndlessShooterGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    // Called by enemies when they die
    void RegisterEnemyKill();
    // Called by destructible objects when destroyed
    void RegisterObjectDestruction();
    // Called by widget when upgrade selection completes
    void NotifyUpgradeComplete();

protected:
    // how many kills before asking
    UPROPERTY(EditDefaultsOnly, Category="Upgrades")
    int32 KillsForUpgrade = 10;

    // how many destructions before asking
    UPROPERTY(EditDefaultsOnly, Category="Upgrades")
    int32 DestructionsForUpgrade = 5;

    // the widget BP you made that has three buttons now
    UPROPERTY(EditDefaultsOnly, Category="UI")
    TSubclassOf<UUpgradePromptWidget> UpgradeWidgetClass;

private:
    int32 KillCount = 0;
    int32 DestructionCount = 0;
    UUpgradePromptWidget* ActiveWidget = nullptr;

    void TryTriggerUpgrade();
    void ShowUpgradePrompt();
};
