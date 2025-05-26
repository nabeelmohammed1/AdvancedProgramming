#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UpgradePromptWidget.h"
#include "EnemySpawner.h"
#include "EndlessShooterGameMode.generated.h"

UCLASS()
class ADVANCEDPROGRAMMING_API AEndlessShooterGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AEndlessShooterGameMode();

    /** Called by enemies when they die; if bIsBoss==true, applies special boss logic */
    UFUNCTION()
    void RegisterEnemyKill(bool bIsBoss = false);

    /** Called by widget when upgrade selection completes */
    UFUNCTION()
    void NotifyUpgradeComplete();
    
    UFUNCTION(BlueprintCallable)
    int32 GetTotalKills() const { return TotalKills; }

    /** Global enemy health scale factor, bumps every KillsPerHealthIncrease */
    UPROPERTY(BlueprintReadOnly, Category="Upgrades|EnemyScaling")
    float EnemyHealthScale = 1.0f;
    
    // References to the spawner(s) in the level
    UPROPERTY()
    TArray<AEnemySpawner*> Spawners;

    /** Spawn an elite every this many kills */
    UPROPERTY(EditDefaultsOnly, Category="Spawning")
    int32 EliteEvery = 10;

    /** Spawn a boss every this many kills */
    UPROPERTY(EditDefaultsOnly, Category="Spawning")
    int32 BossEvery = 50;

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

    // How many have we killed overall?
    int32 TotalKills = 0;
    
    virtual void BeginPlay() override;

private:
    int32 KillCount = 0;
    int32 HealthKillCount = 0;
    UUpgradePromptWidget* ActiveWidget = nullptr;

    void TryTriggerUpgrade();
    void TryHealthIncrease();
    void ShowUpgradePrompt();
};
