// PlayerHUDWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"   // <-- make sure you include this
#include "Components/TextBlock.h"     // <-- and this
#include "PlayerHUDWidget.generated.h"

UCLASS()
class ADVANCEDPROGRAMMING_API UPlayerHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // These UPROPERTYs assume you've named your UMG designer widgets exactly
    // "HealthBar", "DamageText" and "BurstText" and checked "Is Variable".
    UPROPERTY(meta = (BindWidget))
    UProgressBar* HealthBar;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* DamageText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* BurstText;

    /** Call this any time CurrentHealth or MaxHealth changes */
    UFUNCTION(BlueprintCallable)
    void UpdateHealth(float Current, float Max)
    {
        if (HealthBar && Max > 0.f)
        {
            HealthBar->SetPercent(FMath::Clamp(Current / Max, 0.f, 1.f));
        }
    }

    /** Call this whenever ProjectileDamage or BurstCount changes */
    UFUNCTION(BlueprintCallable)
    void UpdateStats(int32 Damage, int32 Burst)
    {
        if (DamageText)
        {
            DamageText->SetText(FText::AsNumber(Damage));
        }
        if (BurstText)
        {
            BurstText->SetText(FText::AsNumber(Burst));
        }
    }
};
