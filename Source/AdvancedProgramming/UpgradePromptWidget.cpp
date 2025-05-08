// UpgradePromptWidget.cpp
#include "UpgradePromptWidget.h"
#include "EndlessShooterGameMode.h"    // ← add this
#include "Components/Button.h"
#include "EndlessShooterGameMode.h"
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
        // apply health buff…
        if (auto PC = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0)))
        {
            PC->MaxHealth += 20.f;
        }
        // remove this widget from viewport:
        RemoveFromParent();
        UGameplayStatics::SetGamePaused(this, false);
    }
}

void UUpgradePromptWidget::OnWeaponClicked()
{
    // Get the game mode to reset state if needed
    if (AEndlessShooterGameMode* GM = Cast<AEndlessShooterGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        // Get the player pawn and cast to your character class
        if (APlayerCharacter* PC = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0)))
        {
            // Increase the player’s projectile damage (tweak the value as you like)
            PC->ProjectileDamage += 10.f;
        }

        // Remove this widget from the viewport
        RemoveFromParent();

        // Unpause the game
        UGameplayStatics::SetGamePaused(this, false);
    }
}
