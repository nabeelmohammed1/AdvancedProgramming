#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

// include these so HealthBar/TextBlock are full types:
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

#include "PlayerHUDWidget.generated.h"

UCLASS()
class ADVANCEDPROGRAMMING_API UPlayerHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // called by PlayerCharacter / UpgradePromptWidget
    UFUNCTION(BlueprintCallable)
    void UpdateHealth(float Current, float Max)
    {
        if (HealthBar)
        {
            HealthBar->SetPercent(Current / Max);
        }
    }

    UFUNCTION(BlueprintCallable)
    void UpdateSprint(float Ratio, float Max)
    {
        if (SprintBar)
        {
            SprintBar->SetPercent(FMath::Clamp(Ratio, 0.0f, 1.0f));
        }
    }

    UFUNCTION(BlueprintCallable)
    void UpdateStats(float Damage, int32 Burst)
    {
        if (DamageText)
        {
            DamageText->SetText(FText::AsNumber(FMath::RoundToInt(Damage)));
        }
        if (BurstText)
        {
            BurstText->SetText(FText::AsNumber(Burst));
        }
    }

protected:
    // these must match your UMG BindWidget names
    UPROPERTY(meta = (BindWidget))
    UProgressBar* HealthBar;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* SprintBar;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* DamageText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* BurstText;
};
