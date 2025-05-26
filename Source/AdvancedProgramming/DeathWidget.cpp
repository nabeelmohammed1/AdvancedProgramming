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
    KillsText->SetText(FText::AsNumber(InKills));
  if (HealthUpgradesText)
    HealthUpgradesText->SetText(FText::AsNumber(InHealthUpgrades));
  if (DamageUpgradesText)
    DamageUpgradesText->SetText(FText::AsNumber(InDamageUpgrades));
  if (BurstUpgradesText)
    BurstUpgradesText->SetText(FText::AsNumber(InBurstUpgrades));
}

void UDeathWidget::OnRestartClicked()
{
  // reload current level
  const FName Current = *UGameplayStatics::GetCurrentLevelName(this, true);
  UGameplayStatics::OpenLevel(this, Current);
}
