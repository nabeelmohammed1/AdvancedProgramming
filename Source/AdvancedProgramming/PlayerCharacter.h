// PlayerCharacter.h
#pragma once

#include "CoreMinimal.h"
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

UCLASS(config=Game)
class ADVANCEDPROGRAMMING_API APlayerCharacter : public ACharacter
{
    GENERATED_BODY()

    /** First-person mesh: arms */
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category=Mesh, meta=(AllowPrivateAccess="true"))
    USkeletalMeshComponent* Mesh1P;

    /** First-person camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="true"))
    UCameraComponent* FirstPersonCameraComponent;

    /** Gun mesh (static mesh view) - assignable in editor */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gun, meta=(AllowPrivateAccess="true"))
    USkeletalMeshComponent* GunMesh;

    /** Muzzle location for spawning projectiles - movable in editor */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gun, meta=(AllowPrivateAccess="true"))
    USceneComponent* MuzzleLocation;

    /** Enhanced Input mapping context */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
    UInputMappingContext* DefaultMappingContext;

    /** Input actions */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
    UInputAction* MoveAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
    UInputAction* LookAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
    UInputAction* JumpAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
    UInputAction* SprintAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
    UInputAction* FireAction;

public:
    APlayerCharacter();

    /** Maximum health */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats)
    float MaxHealth = 100.f;

    /** Current health */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Stats)
    float CurrentHealth;

    /** Damage each projectile deals */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat)
    float ProjectileDamage = 25.f;

    /** Projectile class to spawn */
    UPROPERTY(EditDefaultsOnly, Category=Combat)
    TSubclassOf<AProjectile> ProjectileClass;

    /** Sound to play on fire */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat)
    USoundBase* FireSound;

    /** Animation to play on fire */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat)
    UAnimMontage* FireAnimation;

    /** Fire rate (seconds between shots) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat)
    float FireRate = 0.2f;

protected:
    virtual void NotifyControllerChanged() override;
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void StartSprint();
    void StopSprint();
    void OnFire();
    void ResetSprintCooldown();

private:
    float DefaultMaxWalkSpeed;
    bool bIsSprinting = false;
    bool bCanSprint = true;
    float LastFireTime = 0.f;

    /** Sprint timings */
    UPROPERTY(EditAnywhere, Category=Movement)
    float SprintMultiplier = 2.0f;
    UPROPERTY(EditAnywhere, Category=Movement)
    float SprintDuration = 3.0f;
    UPROPERTY(EditAnywhere, Category=Movement)
    float SprintCooldown = 2.0f;

    FTimerHandle SprintTimerHandle;
    FTimerHandle CooldownTimerHandle;
};
