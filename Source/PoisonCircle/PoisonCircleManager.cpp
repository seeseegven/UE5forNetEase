#include "PoisonCircleManager.h"

#include "DrawDebugHelpers.h"
#include "Engine/DamageEvents.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

APoisonCircleManager::APoisonCircleManager()
{
	PrimaryActorTick.bCanEverTick = true;

	AffectedPawnClass = APawn::StaticClass();
	CurrentCenter = InitialCenter;
	CurrentRadius = InitialRadius;

	FPoisonCircleShrinkEvent FirstShrink;
	FirstShrink.StartRadius = InitialRadius;
	FirstShrink.EndRadius = 5000.0f;
	FirstShrink.TargetCenter = FVector::ZeroVector;
	FirstShrink.DelayBeforeShrink = 15.0f;
	FirstShrink.ShrinkDuration = 60.0f;

	FPoisonCircleShrinkEvent SecondShrink;
	SecondShrink.StartRadius = FirstShrink.EndRadius;
	SecondShrink.EndRadius = 1800.0f;
	SecondShrink.TargetCenter = FVector(1200.0f, 1200.0f, 0.0f);
	SecondShrink.DelayBeforeShrink = 20.0f;
	SecondShrink.ShrinkDuration = 45.0f;

	ShrinkEvents.Add(FirstShrink);
	ShrinkEvents.Add(SecondShrink);
}

void APoisonCircleManager::BeginPlay()
{
	Super::BeginPlay();

	CurrentCenter = InitialCenter;
	CurrentRadius = InitialRadius;

	if (bStartOnBeginPlay)
	{
		StartRound();
	}
}

void APoisonCircleManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bRoundActive)
	{
		return;
	}

	AdvanceShrink(DeltaSeconds);

	DamageElapsedTime += DeltaSeconds;
	if (DamageElapsedTime >= DamageInterval)
	{
		DamageElapsedTime = 0.0f;
		ApplyOutsideCircleDamage();
	}

	if (bDrawDebugCircle)
	{
		DrawCurrentCircle();
	}
}

void APoisonCircleManager::StartRound()
{
	bRoundActive = true;
	CurrentCenter = InitialCenter;
	CurrentRadius = InitialRadius;
	CurrentEventIndex = ShrinkEvents.Num() > 0 ? 0 : INDEX_NONE;
	EventElapsedTime = 0.0f;
	DamageElapsedTime = 0.0f;
	PrepareCurrentEvent();
}

void APoisonCircleManager::StopRound()
{
	bRoundActive = false;
	CurrentEventIndex = INDEX_NONE;
	EventElapsedTime = 0.0f;
	DamageElapsedTime = 0.0f;
}

bool APoisonCircleManager::IsLocationInsideSafeCircle(FVector Location) const
{
	const FVector2D CircleCenter2D(CurrentCenter.X, CurrentCenter.Y);
	const FVector2D Location2D(Location.X, Location.Y);
	return FVector2D::Distance(CircleCenter2D, Location2D) <= CurrentRadius;
}

bool APoisonCircleManager::IsActorOutsideSafeCircle(const AActor* Actor) const
{
	return IsValid(Actor) && !IsLocationInsideSafeCircle(Actor->GetActorLocation());
}

void APoisonCircleManager::AdvanceShrink(float DeltaSeconds)
{
	if (!ShrinkEvents.IsValidIndex(CurrentEventIndex))
	{
		return;
	}

	const FPoisonCircleShrinkEvent& Event = ShrinkEvents[CurrentEventIndex];
	EventElapsedTime += DeltaSeconds;

	if (EventElapsedTime < Event.DelayBeforeShrink)
	{
		return;
	}

	const float ShrinkTime = EventElapsedTime - Event.DelayBeforeShrink;
	const float Alpha = FMath::Clamp(ShrinkTime / FMath::Max(Event.ShrinkDuration, 0.1f), 0.0f, 1.0f);

	CurrentRadius = FMath::Lerp(EventStartRadius, Event.EndRadius, Alpha);
	CurrentCenter = FMath::Lerp(EventStartCenter, Event.TargetCenter, Alpha);

	if (Alpha >= 1.0f)
	{
		CurrentEventIndex++;
		EventElapsedTime = 0.0f;
		PrepareCurrentEvent();
	}
}

void APoisonCircleManager::ApplyOutsideCircleDamage()
{
	UWorld* World = GetWorld();
	if (!World || !AffectedPawnClass)
	{
		return;
	}

	TArray<AActor*> Pawns;
	UGameplayStatics::GetAllActorsOfClass(World, AffectedPawnClass, Pawns);

	const float DamageAmount = DamagePerSecond * DamageInterval;
	for (AActor* PawnActor : Pawns)
	{
		if (!IsActorOutsideSafeCircle(PawnActor))
		{
			continue;
		}

		UGameplayStatics::ApplyDamage(PawnActor, DamageAmount, nullptr, this, DamageTypeClass);
	}
}

void APoisonCircleManager::DrawCurrentCircle() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FVector DrawCenter(CurrentCenter.X, CurrentCenter.Y, GetActorLocation().Z);
	DrawDebugCircle(
		World,
		DrawCenter,
		CurrentRadius,
		DebugSegments,
		FColor::Green,
		false,
		0.0f,
		0,
		DebugLineThickness,
		FVector::ForwardVector,
		FVector::RightVector,
		false);
}

void APoisonCircleManager::PrepareCurrentEvent()
{
	if (!ShrinkEvents.IsValidIndex(CurrentEventIndex))
	{
		return;
	}

	EventStartRadius = CurrentRadius;
	EventStartCenter = CurrentCenter;
}
