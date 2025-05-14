// UpgradePromptWidget.cpp

#include "UpgradePromptWidget.h"
#include "EndlessShooterGameMode.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"

bool UUpgradePromptWidget::Initialize()
{
    if (!Super::Initialize()) return false;

    if (HealthButton)
        HealthButton->OnClicked.AddDynamic(this, &UUpgradePromptWidget::OnHealthClicked);
    if (WeaponButton)
        WeaponButton->OnClicked.AddDynamic(this, &UUpgradePromptWidget::OnWeaponClicked);
    if (BurstButton)
        BurstButton->OnClicked.AddDynamic(this, &UUpgradePromptWidget::OnBurstClicked);

    return true;
}

void UUpgradePromptWidget::OnHealthClicked()
{
    if (auto GM = Cast<AEndlessShooterGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        if (auto PC = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0)))
        {
            // apply health upgrade
            PC->MaxHealth    += 20.f;
            PC->CurrentHealth = PC->MaxHealth;

            // update HUD
            if (PC->HUDWidget)
            {
                PC->HUDWidget->UpdateHealth(PC->CurrentHealth, PC->MaxHealth);
                PC->HUDWidget->UpdateStats(PC->ProjectileDamage, PC->ShotsPerBurst);
            }
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
            // apply damage upgrade
            PC->ProjectileDamage += 10.f;

            // update HUD
            if (PC->HUDWidget)
            {
                PC->HUDWidget->UpdateStats(PC->ProjectileDamage, PC->ShotsPerBurst);
                PC->HUDWidget->UpdateHealth(PC->CurrentHealth, PC->MaxHealth);
            }
        }

        GM->NotifyUpgradeComplete();
        RemoveFromParent();
    }
}

void UUpgradePromptWidget::OnBurstClicked()
{
    if (auto GM = Cast<AEndlessShooterGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        if (auto PC = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0)))
        {
            // increment burst (1 → 2 → 3)
            PC->ShotsPerBurst = FMath::Clamp(PC->ShotsPerBurst + 1, 1, 3);

            // update HUD
            if (PC->HUDWidget)
            {
                PC->HUDWidget->UpdateStats(PC->ProjectileDamage, PC->ShotsPerBurst);
                PC->HUDWidget->UpdateHealth(PC->CurrentHealth, PC->MaxHealth);
            }
        }

        GM->NotifyUpgradeComplete();
        RemoveFromParent();
    }
}
