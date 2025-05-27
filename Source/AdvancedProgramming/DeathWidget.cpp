#include "DeathWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

bool UDeathWidget::Initialize()
{
  if (!Super::Initialize()) return false;

  if (RestartButton)
    RestartButton->OnClicked.AddDynamic(this, &UDeathWidget::OnRestartClicked);

  return true;
}

void UDeathWidget::SetStats(int32 InKills, int32 InHealthUpgrades, int32 InDamageUpgrades, int32 InBurstUpgrades)
{
  if (KillsText)
    FString KillString = FString::Printf(TEXT("Total Kills = %d"), InKills);
    KillsText->SetText(FText::AsNumber(InKills));
  if (HealthUpgradesText)
    FString HealthString = FString::Printf(TEXT("Total Health Upgrades = %d"), InHealthUpgrades);
    HealthUpgradesText->SetText(FText::AsNumber(InHealthUpgrades));
  if (DamageUpgradesText)
    FString DamageString = FString::Printf(TEXT("Total Health Upgrades = %d"), InDamageUpgrades);
    DamageUpgradesText->SetText(FText::AsNumber(InDamageUpgrades));
  if (BurstUpgradesText)
    FString BurstString = FString::Printf(TEXT("Total Health Upgrades = %d"), InBurstUpgrades);
    BurstUpgradesText->SetText(FText::AsNumber(InBurstUpgrades));
}

void UDeathWidget::OnRestartClicked()
{
  // reload current level
  const FName Current = *UGameplayStatics::GetCurrentLevelName(this, true);
  UGameplayStatics::OpenLevel(this, Current);
}
