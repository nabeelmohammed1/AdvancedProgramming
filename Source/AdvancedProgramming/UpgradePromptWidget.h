// UpgradePromptWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UpgradePromptWidget.generated.h"

class UButton;

UCLASS()
class ADVANCEDPROGRAMMING_API UUpgradePromptWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;

protected:
    // Bind these exact names in your UMG Designer
    UPROPERTY(meta=(BindWidget))
    UButton* HealthButton;

    UPROPERTY(meta=(BindWidget))
    UButton* WeaponButton;

    UPROPERTY(meta=(BindWidget))
    UButton* BurstButton;

private:
    UFUNCTION()
    void OnHealthClicked();

    UFUNCTION()
    void OnWeaponClicked();

    UFUNCTION()
    void OnBurstClicked();
};
