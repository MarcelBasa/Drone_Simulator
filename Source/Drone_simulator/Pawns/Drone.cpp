#include "Drone.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Drone_simulator/Controllers/DroneController.h"


ADrone::ADrone()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));
	SetRootComponent(DefaultRoot);

	DroneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Drone mesh"));
	DroneMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DroneMesh->SetupAttachment(RootComponent);

	Wing1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wing 1 mesh"));
	Wing1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Wing1->SetupAttachment(DroneMesh);

	Wing2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wing 2 mesh"));
	Wing2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Wing2->SetupAttachment(DroneMesh);

	Wing3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wing 3 mesh"));
	Wing3->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Wing3->SetupAttachment(DroneMesh);

	Wing4 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wing 4 mesh"));
	Wing4->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Wing4->SetupAttachment(DroneMesh);
	

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);
	Camera->bUsePawnControlRotation = true;

	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
}

void ADrone::BeginPlay()
{
	Super::BeginPlay();

	if (FlySound)
	{
		UGameplayStatics::PlaySound2D(
			GetWorld(),
			FlySound
		);
	}
	// DEBUG
	//MovementComponent->Deactivate();
	//MoveToTarget(FVector(0, -4000.f, 10000.f));
}

void ADrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsMovingToTarget)
	{
		FVector CurrentLocation = GetActorLocation();

		// Sprawd�, czy dron jest blisko celu, aby zatrzyma� ruch
		if (FVector::Dist(CurrentLocation, TargetLocation) <= 10.f)  // Tolerancja 10 jednostek
		{
			bIsMovingToTarget = false;
			UE_LOG(LogTemp, Warning, TEXT("Pass"));
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("Flying"));
			// Interpolacja z r�wn� pr�dko�ci�
			FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaTime, MovementSpeed);
			SetActorLocation(NewLocation);
		}
	}
}

void ADrone::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ThisClass::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ThisClass::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ThisClass::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ThisClass::LookUp);
	PlayerInputComponent->BindAction(TEXT("Pause"), IE_Pressed, this, &ThisClass::PauseButtonClick);
}

void ADrone::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);
	}
}

void ADrone::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Value);
	}
}

void ADrone::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ADrone::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void ADrone::MoveToTarget(const FVector& Target)
{
	TargetLocation = Target;
	bIsMovingToTarget = true;
}

void ADrone::PauseButtonClick()
{
	DroneContoller = DroneContoller == nullptr ? Cast<ADroneController>(Controller) : DroneContoller;
	if(DroneContoller)
		DroneContoller->HandleSetPauseMenu();
}