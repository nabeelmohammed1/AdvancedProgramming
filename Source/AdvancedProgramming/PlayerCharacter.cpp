// PlayerCharacter.cpp
#include "PlayerCharacter.h"
#include "Projectile.h"
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
    GunMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunMesh"));
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

    FVector Forward = FirstPersonCameraComponent->GetComponentRotation().Vector();
    FVector SpawnLocBase = MuzzleLocation->GetComponentLocation();
    FRotator SpawnRot = FirstPersonCameraComponent->GetComponentRotation();

    for (int32 i = 0; i < ShotsPerBurst; ++i)
    {
        FVector Offset = FVector(0.f, (i - (ShotsPerBurst-1)/2.0f) * 5.f, 0.f);
        FVector SpawnLoc = SpawnLocBase + FirstPersonCameraComponent->GetRightVector() * Offset.Y;

        FActorSpawnParameters Params;
        Params.Owner = this;
        Params.Instigator = this;

        AProjectile* Proj = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLoc, SpawnRot, Params);
        if (Proj)
        {
            Proj->Damage = ProjectileDamage;
            if (auto MoveComp = Proj->FindComponentByClass<UProjectileMovementComponent>())
                MoveComp->Velocity = Forward * MoveComp->InitialSpeed;
        }
    }

    LastFireTime = Now;
    if (FireSound) UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
    if (FireAnimation && Mesh1P->GetAnimInstance())
        Mesh1P->GetAnimInstance()->Montage_Play(FireAnimation, 1.f);
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
