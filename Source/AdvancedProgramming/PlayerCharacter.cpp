// PlayerCharacter.cpp
#include "PlayerCharacter.h"
#include "Projectile.h"
#include "PlayerHUDWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/PrimitiveComponent.h"

APlayerCharacter::APlayerCharacter()
{
    GetCapsuleComponent()->InitCapsuleSize(55.f, 96.f);

    FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
    FirstPersonCameraComponent->SetRelativeLocation(FVector(0.f, 0.f, 64.f));
    FirstPersonCameraComponent->bUsePawnControlRotation = true;

    Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmMesh1P"));
    Mesh1P->SetOnlyOwnerSee(true);
    Mesh1P->SetupAttachment(FirstPersonCameraComponent);
    Mesh1P->bCastDynamicShadow = false;
    Mesh1P->CastShadow = false;

    GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
    GunMesh->SetupAttachment(Mesh1P, TEXT("GripPoint"));
    GunMesh->SetOnlyOwnerSee(true);
    GunMesh->bCastDynamicShadow = false;
    GunMesh->CastShadow = false;

    MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
    MuzzleLocation->SetupAttachment(GunMesh);
    MuzzleLocation->SetRelativeLocation(FVector(0.f, 50.f, 10.f));
}

void APlayerCharacter::NotifyControllerChanged()
{
    Super::NotifyControllerChanged();
    if (APlayerController* PC = Cast<APlayerController>(Controller))
        if (auto Sub = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
            Sub->AddMappingContext(DefaultMappingContext, 0);
}

void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
    DefaultMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
    bCanSprint = true;
    CurrentHealth = MaxHealth;
    // Create and add HUD
    if (HUDWidgetClass)
        {
            if (APlayerController* PC = Cast<APlayerController>(GetController()))
            {
                HUDWidget = CreateWidget<UPlayerHUDWidget>(PC, HUDWidgetClass);
                if (HUDWidget)
                {
                    HUDWidget->AddToViewport();
                    
                    // initialize with your current values
                    HUDWidget->UpdateHealth(CurrentHealth, MaxHealth);
                    HUDWidget->UpdateStats(ProjectileDamage, BurstCount);
                }
            }
        }
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    if (auto Enhanced = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        Enhanced->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
        Enhanced->BindAction(JumpAction, ETriggerEvent::Canceled, this, &ACharacter::StopJumping);
        Enhanced->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
        Enhanced->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
        Enhanced->BindAction(SprintAction, ETriggerEvent::Started, this, &APlayerCharacter::StartSprint);
        Enhanced->BindAction(SprintAction, ETriggerEvent::Canceled, this, &APlayerCharacter::StopSprint);
        Enhanced->BindAction(FireAction, ETriggerEvent::Started, this, &APlayerCharacter::OnFire);
    }
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
    FVector2D Dir = Value.Get<FVector2D>();
    AddMovementInput(GetActorForwardVector(), Dir.Y);
    AddMovementInput(GetActorRightVector(), Dir.X);
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
    FVector2D Delta = Value.Get<FVector2D>();
    AddControllerYawInput(Delta.X);
    AddControllerPitchInput(Delta.Y);
}

void APlayerCharacter::OnFire()
{
    float Now = GetWorld()->GetTimeSeconds();
    if (!ProjectileClass || Now - LastFireTime < FireRate) return;

    // initialize burst
    BurstShotsRemaining = ShotsPerBurst;

    // fire first shot immediately
    FireOneBurstShot();

    // schedule remaining shots
    if (BurstShotsRemaining > 0)
    {
        GetWorldTimerManager().SetTimer(BurstTimerHandle, this, &APlayerCharacter::FireOneBurstShot, BurstShotInterval, true);
    }

    LastFireTime = Now;

    if (FireSound)
        UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
    if (FireAnimation && Mesh1P->GetAnimInstance())
        Mesh1P->GetAnimInstance()->Montage_Play(FireAnimation, 1.f);
}

void APlayerCharacter::FireOneBurstShot()
{
    if (BurstShotsRemaining <= 0)
    {
        GetWorldTimerManager().ClearTimer(BurstTimerHandle);
        return;
    }

    FVector Forward = FirstPersonCameraComponent->GetComponentRotation().Vector();
    FVector SpawnLoc = MuzzleLocation->GetComponentLocation();
    FRotator SpawnRot = FirstPersonCameraComponent->GetComponentRotation();

    FActorSpawnParameters Params;
    Params.Owner = this;
    Params.Instigator = this;

    if (AProjectile* Proj = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLoc, SpawnRot, Params))
    {
        // ignore collision with self
        if (auto CollComp = Cast<UPrimitiveComponent>(Proj->GetRootComponent()))
            CollComp->IgnoreActorWhenMoving(this, true);

        // set damage and velocity
        Proj->Damage = ProjectileDamage;
        if (auto MoveComp = Proj->FindComponentByClass<UProjectileMovementComponent>())
            MoveComp->Velocity = Forward * MoveComp->InitialSpeed;
    }

    BurstShotsRemaining--;
    if (BurstShotsRemaining <= 0)
    {
        GetWorldTimerManager().ClearTimer(BurstTimerHandle);
    }
}

void APlayerCharacter::StartSprint()
{
    if (!bCanSprint || bIsSprinting) return;
    bIsSprinting = true; bCanSprint = false;
    GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed * SprintMultiplier;
    GetWorldTimerManager().SetTimer(SprintTimerHandle, this, &APlayerCharacter::StopSprint, SprintDuration, false);
}

void APlayerCharacter::StopSprint()
{
    if (!bIsSprinting) return;
    bIsSprinting = false;
    GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;
    GetWorldTimerManager().SetTimer(CooldownTimerHandle, this, &APlayerCharacter::ResetSprintCooldown, SprintCooldown, false);
}

void APlayerCharacter::ResetSprintCooldown() { bCanSprint = true; }

float APlayerCharacter::TakeDamage(float DamageAmount,
                                   const FDamageEvent& DamageEvent,
                                   AController* EventInstigator,
                                   AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    if (ActualDamage <= 0.f)
        return 0.f;
    
    CurrentHealth -= ActualDamage;
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red,
                                         FString::Printf(TEXT("Player took %.1f damage. HP: %.1f"), ActualDamage, CurrentHealth));
    }
    
    if (HUDWidget)
    {
        HUDWidget->UpdateHealth(CurrentHealth, MaxHealth);
    }

    
    if (CurrentHealth <= 0.f)
    {
        Die();
    }
    
    return ActualDamage;
}
    
    void APlayerCharacter::Die()
    {
        if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
            PC->DisableInput(PC);
        }
        // Optional: play death animation, ragdoll, UI, etc.
        Destroy();
    }
