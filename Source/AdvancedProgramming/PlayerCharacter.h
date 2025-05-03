#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

// Forward declarations
class UCameraComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class AProjectile;
class USoundBase;
class UAnimMontage;

UCLASS(config=Game)
class ADVANCEDPROGRAMMING_API APlayerCharacter : public ACharacter
{
    GENERATED_BODY()

    /** 1st-person arms mesh (only visible to owning player) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh, meta=(AllowPrivateAccess="true"))
    USkeletalMeshComponent* Mesh1P;

    /** First-person camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="true"))
    UCameraComponent* FirstPersonCameraComponent;

    /** Gun mesh (first-person view) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Gun, meta=(AllowPrivateAccess="true"))
    USkeletalMeshComponent* GunMesh;

    /** Muzzle location for spawning projectiles */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Gun, meta=(AllowPrivateAccess="true"))
    USceneComponent* MuzzleLocation;

    /** Enhanced Input mapping context */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
    UInputMappingContext* DefaultMappingContext;

    /** Move/look/jump/sprint/fire actions */
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

    /** Sprint settings */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess="true"))
    float SprintMultiplier = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess="true"))
    float SprintDuration = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess="true"))
    float SprintCooldown = 2.0f;

    /** Projectile to spawn */
    UPROPERTY(EditDefaultsOnly, Category=Gameplay)
    TSubclassOf<AProjectile> ProjectileClass;

    /** Sound to play on fire */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay, meta=(AllowPrivateAccess="true"))
    USoundBase* FireSound;

    /** Animation to play on fire */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay, meta=(AllowPrivateAccess="true"))
    UAnimMontage* FireAnimation;

    /** Fire rate (seconds between shots) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay, meta=(AllowPrivateAccess="true"))
    float FireRate = 0.2f;

public:
    APlayerCharacter();

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
    bool  bIsSprinting = false;
    bool  bCanSprint   = true;
    float LastFireTime = 0.f;

    FTimerHandle SprintTimerHandle;
    FTimerHandle CooldownTimerHandle;
};
