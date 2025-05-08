// UpgradePromptWidget.cpp
#include "UpgradePromptWidget.h"
#include "Components/Button.h"
#include "EndlessShooterGameMode.h"
#include "Kismet/GameplayStatics.h"

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
        // e.g. increase player's max health
        if (auto PC = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0)))
        {
            PC->MaxHealth += 20.f;
        }
        GM->ActiveWidget->RemoveFromParent();
        UGameplayStatics::SetGamePaused(this, false);
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
        GM->ActiveWidget->RemoveFromParent();
        UGameplayStatics::SetGamePaused(this, false);
    }
}
