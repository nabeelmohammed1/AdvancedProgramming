// UpgradePromptWidget.cpp
#include "UpgradePromptWidget.h"
#include "EndlessShooterGameMode.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"

bool UUpgradePromptWidget::Initialize()
{
    if (!Super::Initialize()) return false;

    if (HealthButton)
        HealthButton->OnClicked.AddDynamic(this, &UUpgradePromptWidget::OnHealthClicked);
    if (WeaponButton)
        WeaponButton->OnClicked.AddDynamic(this, &UUpgradePromptWidget::OnWeaponClicked);

    return true;
}

void UUpgradePromptWidget::OnHealthClicked()
{
    if (auto GM = Cast<AEndlessShooterGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        if (auto PC = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0)))
        {
            PC->MaxHealth += 20.f;
            PC->CurrentHealth = PC->MaxHealth;
        }
        GM->NotifyUpgradeComplete();
        RemoveFromParent();
    }
}

void UUpgradePromptWidget::OnWeaponClicked()
{
    if (auto GM = Cast<AEndlessShooterGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        if (auto PC = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0)))
        {
            PC->ProjectileDamage += 10.f;
        }
        GM->NotifyUpgradeComplete();
        RemoveFromParent();
    }
}

