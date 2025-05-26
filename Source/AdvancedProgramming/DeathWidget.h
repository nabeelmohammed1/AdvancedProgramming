#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeathWidget.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class ADVANCEDPROGRAMMING_API UDeathWidget : public UUserWidget
{
  GENERATED_BODY()

public:
  virtual bool Initialize() override;

  /** Populate all the text fields */
  UFUNCTION(BlueprintCallable)
  void SetStats(int32 InKills, int32 InHealthUpgrades, int32 InDamageUpgrades, int32 InBurstUpgrades);

protected:
  /** Bound from your UMG Designer */
  UPROPERTY(meta = (BindWidget))
  UTextBlock* KillsText;

  UPROPERTY(meta = (BindWidget))
  UTextBlock* HealthUpgradesText;

  UPROPERTY(meta = (BindWidget))
  UTextBlock* DamageUpgradesText;

  UPROPERTY(meta = (BindWidget))
  UTextBlock* BurstUpgradesText;

  UPROPERTY(meta = (BindWidget))
  UButton* RestartButton;

private:
  UFUNCTION()
  void OnRestartClicked();
};
