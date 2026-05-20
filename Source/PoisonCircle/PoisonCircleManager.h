#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PoisonCircleManager.generated.h"

USTRUCT(BlueprintType)
struct FPoisonCircleShrinkEvent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poison Circle", meta = (DeprecatedProperty, DeprecationMessage = "StartRadius is no longer used. Each shrink event starts from the current safe circle."))
	float StartRadius = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poison Circle")
	float EndRadius = 2500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poison Circle")
	FVector TargetCenter = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poison Circle", meta = (ClampMin = "0.0"))
	float DelayBeforeShrink = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poison Circle", meta = (ClampMin = "0.1"))
	float ShrinkDuration = 45.0f;
};

UCLASS(Blueprintable)
class POISONCIRCLE_API APoisonCircleManager : public AActor
{
	GENERATED_BODY()

public:
	APoisonCircleManager();

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "Poison Circle")
	void StartRound();

	UFUNCTION(BlueprintCallable, Category = "Poison Circle")
	void StopRound();

	UFUNCTION(BlueprintPure, Category = "Poison Circle")
	bool IsRoundActive() const { return bRoundActive; }

	UFUNCTION(BlueprintPure, Category = "Poison Circle")
	float GetCurrentRadius() const { return CurrentRadius; }

	UFUNCTION(BlueprintPure, Category = "Poison Circle")
	FVector GetCurrentCenter() const { return CurrentCenter; }

	UFUNCTION(BlueprintPure, Category = "Poison Circle")
	bool IsLocationInsideSafeCircle(FVector Location) const;

	UFUNCTION(BlueprintPure, Category = "Poison Circle")
	bool IsActorOutsideSafeCircle(const AActor* Actor) const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poison Circle")
	bool bStartOnBeginPlay = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poison Circle")
	FVector InitialCenter = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poison Circle", meta = (ClampMin = "1.0"))
	float InitialRadius = 9000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poison Circle")
	TArray<FPoisonCircleShrinkEvent> ShrinkEvents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poison Circle|Damage", meta = (ClampMin = "0.0"))
	float DamagePerSecond = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poison Circle|Damage", meta = (ClampMin = "0.05"))
	float DamageInterval = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poison Circle|Damage")
	TSubclassOf<APawn> AffectedPawnClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poison Circle|Damage")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poison Circle|Debug")
	bool bDrawDebugCircle = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poison Circle|Debug")
	bool bDrawDebugTargetCircle = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poison Circle|Debug", meta = (ClampMin = "8"))
	int32 DebugSegments = 96;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poison Circle|Debug", meta = (ClampMin = "0.0"))
	float DebugLineThickness = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poison Circle|Debug")
	FColor SafeZoneColor = FColor::Cyan;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poison Circle|Debug")
	FColor TargetZoneColor = FColor::White;

private:
	void AdvanceShrink(float DeltaSeconds);
	void ApplyOutsideCircleDamage();
	void DrawCurrentCircle() const;
	void DrawTargetCircle() const;
	void PrepareCurrentEvent();

	int32 CurrentEventIndex = INDEX_NONE;
	float CurrentRadius = 0.0f;
	FVector CurrentCenter = FVector::ZeroVector;
	float EventElapsedTime = 0.0f;
	float EventStartRadius = 0.0f;
	FVector EventStartCenter = FVector::ZeroVector;
	float DamageElapsedTime = 0.0f;
	bool bRoundActive = false;
};
