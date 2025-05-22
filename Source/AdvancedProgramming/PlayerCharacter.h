// PlayerCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "PlayerHUDWidget.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class UCameraComponent;
class USceneComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class AProjectile;
class USoundBase;
class UAnimMontage;
class UStaticMeshComponent;
class UPrimitiveComponent;

UCLASS(config = Game)
class ADVANCEDPROGRAMMING_API APlayerCharacter : public ACharacter
{
    GENERATED_BODY()

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* Mesh1P;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FirstPersonCameraComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gun, meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* GunMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gun, meta = (AllowPrivateAccess = "true"))
    USceneComponent* MuzzleLocation;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* JumpAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* SprintAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* FireAction;

public:
    APlayerCharacter();

    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
    float MaxHealth = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stats)
    float CurrentHealth;

    virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

    void Die();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
    float ProjectileDamage = 25.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (ClampMin = "1", ClampMax = "3"))
    int32 ShotsPerBurst = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
    float BurstShotInterval = 0.1f;

    UPROPERTY(EditDefaultsOnly, Category = Combat)
    TSubclassOf<AProjectile> ProjectileClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
    USoundBase* FireSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
    UAnimMontage* FireAnimation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
    float FireRate = 0.2f;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UPlayerHUDWidget> HUDWidgetClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    UPlayerHUDWidget* HUDWidget;

protected:
    virtual void NotifyControllerChanged() override;
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void StartSprint();
    void StopSprint();
    void OnFire();
    void FireOneBurstShot();
    void ResetSprintCooldown();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    int32 BurstCount = 1;

private:
    float DefaultMaxWalkSpeed;
    bool bIsSprinting = false;
    bool bCanSprint = true;
    float LastFireTime = 0.f;

    UPROPERTY(EditAnywhere, Category = Movement)
    float SprintMultiplier = 2.0f;

    UPROPERTY(EditAnywhere, Category = Movement)
    float SprintDuration = 3.0f;

    UPROPERTY(EditAnywhere, Category = Movement)
    float SprintCooldown = 2.0f;

    int32 BurstShotsRemaining = 0;
    FTimerHandle BurstTimerHandle;

    FTimerHandle SprintTimerHandle;
    FTimerHandle CooldownTimerHandle;
};