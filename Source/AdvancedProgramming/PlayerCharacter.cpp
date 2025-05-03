// PlayerCharacter.cpp

#include "PlayerCharacter.h"
#include "Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/ProjectileMovementComponent.h"  // for UProjectileMovementComponent

APlayerCharacter::APlayerCharacter()
{
    // Capsule setup
    GetCapsuleComponent()->InitCapsuleSize(55.f, 96.f);

    // Camera
    FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
    FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f));
    FirstPersonCameraComponent->bUsePawnControlRotation = true;

    // Arms mesh
    Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh1P"));
    Mesh1P->SetOnlyOwnerSee(true);
    Mesh1P->SetupAttachment(FirstPersonCameraComponent);
    Mesh1P->bCastDynamicShadow = false;
    Mesh1P->CastShadow        = false;
    Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

    // Gun mesh
    GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
    GunMesh->SetOnlyOwnerSee(true);
    GunMesh->bCastDynamicShadow = false;
    GunMesh->CastShadow        = false;
    GunMesh->SetupAttachment(Mesh1P, TEXT("GripPoint"));  // assumes a socket named GripPoint on your arms mesh

    // Muzzle location
    MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
    MuzzleLocation->SetupAttachment(GunMesh);
    MuzzleLocation->SetRelativeLocation(FVector(0.f, 50.f, 10.f));
}

void APlayerCharacter::NotifyControllerChanged()
{
    Super::NotifyControllerChanged();
    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        if (auto Sub = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
            Sub->AddMappingContext(DefaultMappingContext, 0);
    }
}

void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    DefaultMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
    bCanSprint         = true;

    // Ensure Enhanced-Input context is active
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (auto Sub = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
            Sub->AddMappingContext(DefaultMappingContext, 0);
    }
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* InputComponent)
{
    Super::SetupPlayerInputComponent(InputComponent);

    if (auto EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
    {
        // Jump
        EnhancedInput->BindAction(JumpAction,   ETriggerEvent::Started,   this, &ACharacter::Jump);
        EnhancedInput->BindAction(JumpAction,   ETriggerEvent::Canceled,  this, &ACharacter::StopJumping);

        // Move & Look
        EnhancedInput->BindAction(MoveAction,   ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
        EnhancedInput->BindAction(LookAction,   ETriggerEvent::Triggered, this, &APlayerCharacter::Look);

        // Sprint
        EnhancedInput->BindAction(SprintAction, ETriggerEvent::Started,   this, &APlayerCharacter::StartSprint);
        EnhancedInput->BindAction(SprintAction, ETriggerEvent::Canceled,  this, &APlayerCharacter::StopSprint);

        // Fire
        EnhancedInput->BindAction(FireAction,   ETriggerEvent::Started,   this, &APlayerCharacter::OnFire);
    }
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
    FVector2D Dir = Value.Get<FVector2D>();
    AddMovementInput(GetActorForwardVector(), Dir.Y);
    AddMovementInput(GetActorRightVector(),   Dir.X);
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
    if (!ProjectileClass || Now - LastFireTime < FireRate)
        return;

    UWorld* World = GetWorld();
    if (!World) return;

    // 1) Compute aim from camera
    FRotator FireRotation = FirstPersonCameraComponent->GetComponentRotation();
    FVector  FireLocation = MuzzleLocation->GetComponentLocation();

    // —— INSERT THIS BLOCK BEFORE SpawnActor —— //

    // A) Tell the projectile who spawned it
    FActorSpawnParameters Params;
    Params.Owner      = this;
    Params.Instigator = this;

    // B) Nudge the spawn point out in front of the player
    const float SpawnOffset = 20.f;
    FVector Forward       = FireRotation.Vector();
    FVector SpawnLoc      = FireLocation + Forward * SpawnOffset;
    FRotator SpawnRot     = FireRotation;

    // —— END INSERT —— //

    // now actually spawn, using our Params
    AProjectile* SpawnedProj = World->SpawnActor<AProjectile>(
        ProjectileClass,
        SpawnLoc,
        SpawnRot,
        Params
    );

    if (SpawnedProj)
    {
        if (auto MoveComp = SpawnedProj->FindComponentByClass<UProjectileMovementComponent>())
        {
            MoveComp->Velocity = Forward * MoveComp->InitialSpeed;
        }
    }

    LastFireTime = Now;

    // play sound / animation…
    if (FireSound)
        UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());

    if (FireAnimation)
    {
        if (auto AnimInst = Mesh1P->GetAnimInstance())
            AnimInst->Montage_Play(FireAnimation, 1.f);
    }
}


void APlayerCharacter::StartSprint()
{
    if (!bCanSprint || bIsSprinting) return;
    bIsSprinting = true;
    bCanSprint   = false;
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

void APlayerCharacter::ResetSprintCooldown()
{
    bCanSprint = true;
}
