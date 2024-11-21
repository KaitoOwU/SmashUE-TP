// Fill out your copyright notice in the Description page of Project Settings.

#include "Camera/CameraWorldSubsystem.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraFollowTarget.h"
#include "Camera/CameraSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void UCameraWorldSubsystem::PostInitialize()
{
	Super::PostInitialize();
}

void UCameraWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	CameraMain = FindCameraByTag(FName(GetDefault<UCameraSettings>()->CameraMainTag));

	AActor* CameraBoundsActor = FindCameraBoundsActor();
	if(CameraBoundsActor != nullptr)
	{
		InitCameraBounds(CameraBoundsActor);
	}

	InitCameraZoomParameters();
}

void UCameraWorldSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TickUpdateCameraZoom(DeltaTime);
	TickUpdateCameraPosition(DeltaTime);
}

void UCameraWorldSubsystem::AddFollowTarget(UObject* FollowTarget)
{
	FollowTargets.Add(FollowTarget);
}

void UCameraWorldSubsystem::RemoveFollowTarget(UObject* FollowTarget)
{
	FollowTargets.Remove(FollowTarget);
}

AActor* UCameraWorldSubsystem::FindCameraBoundsActor()
{
	TArray<AActor*> CameraBounds;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(GetDefault<UCameraSettings>()->CameraBoundsTag), CameraBounds);
	return CameraBounds.Num() > 0 ? CameraBounds[0] : nullptr;
}

void UCameraWorldSubsystem::InitCameraBounds(AActor* CameraBoundsActor)
{
	FVector BoundsCenter;
	FVector BoundsExtents;
	CameraBoundsActor->GetActorBounds(false, BoundsCenter, BoundsExtents);
	
	CameraBoundsYProjectionCenter = BoundsCenter.Y;
	CameraBoundsMin = FVector2D(BoundsCenter.X - BoundsExtents.X, BoundsCenter.Z - BoundsExtents.Z);
	CameraBoundsMax = FVector2D(BoundsCenter.X + BoundsExtents.X, BoundsCenter.Z + BoundsExtents.Z);
}

void UCameraWorldSubsystem::ClampPositionIntoCameraBounds(FVector& Position)
{
	FVector2D ViewportBoundsMin,ViewportBoundsMax;
	GetViewportBounds(ViewportBoundsMin, ViewportBoundsMax);
	
	FVector WorldBoundsMin = CalculateWorldPositionFromViewportPosition(ViewportBoundsMin);
	FVector WorldBoundsMax = CalculateWorldPositionFromViewportPosition(ViewportBoundsMax);

	FVector2D WorldBoundSize(
			(WorldBoundsMax.X - WorldBoundsMin.X) / 2.f,
			(WorldBoundsMin.Z - WorldBoundsMax.Z) / 2.f
		);

	/*float BoundsXMin = CameraBoundsMin.X + WorldBoundSize.X;
	float BoundsZMin = CameraBoundsMin.Y + WorldBoundSize.Y;
	float BoundsXMax = CameraBoundsMax.X - WorldBoundSize.X;
	float BoundsZMax = CameraBoundsMax.Y - WorldBoundSize.Y;

	Position.X = FMath::Clamp(Position.X, BoundsXMin, BoundsXMax);
	Position.Z = FMath::Clamp(Position.Z, BoundsZMin, BoundsZMax);*/
	
	if(Position.X - WorldBoundSize.X < CameraBoundsMin.X)
	{
		Position.X = CameraBoundsMin.X + WorldBoundSize.X;
	} else if(Position.X + WorldBoundSize.X > CameraBoundsMax.X)
	{
		Position.X = CameraBoundsMax.X - WorldBoundSize.X ;
	}

	if(Position.Z - WorldBoundSize.Y < CameraBoundsMin.Y)
	{
		Position.Z = CameraBoundsMin.Y + WorldBoundSize.Y;
	} else if(Position.Z + WorldBoundSize.Y > CameraBoundsMax.Y)
	{
		Position.Z = CameraBoundsMax.Y - WorldBoundSize.Y;
	}
}

void UCameraWorldSubsystem::GetViewportBounds(FVector2D& OutViewportBoundsMin, FVector2D& OutViewportBoundsMax)
{
	UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
	if(ViewportClient == nullptr) return;

	FViewport* Viewport = ViewportClient->Viewport;
	if(Viewport == nullptr) return;

	FIntRect ViewRect(
		Viewport->GetInitialPositionXY(),
		Viewport->GetInitialPositionXY() + Viewport->GetSizeXY()
	);
	FIntRect ViewportRect = Viewport->CalculateViewExtents(CameraMain->AspectRatio, ViewRect);

	OutViewportBoundsMin.X = ViewportRect.Min.X;
	OutViewportBoundsMin.Y = ViewportRect.Min.Y;

	OutViewportBoundsMax.X = ViewportRect.Max.X;
	OutViewportBoundsMax.Y = ViewportRect.Max.Y;
}

FVector UCameraWorldSubsystem::CalculateWorldPositionFromViewportPosition(const FVector2D& ViewportPosition)
{
	if(CameraMain == nullptr) return FVector::Zero();

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if(PlayerController == nullptr) return FVector::Zero();

	float YDistanceToCenter = CameraMain->GetComponentLocation().Y - CameraBoundsYProjectionCenter;

	FVector CameraWorldProjectDir;
	FVector WorldPosition;
	UGameplayStatics::DeprojectScreenToWorld(
		PlayerController,
		ViewportPosition,
		WorldPosition,
		CameraWorldProjectDir
	);

	WorldPosition += CameraWorldProjectDir * YDistanceToCenter;
	return WorldPosition;
}

void UCameraWorldSubsystem::TickUpdateCameraZoom(float DeltaTime)
{

	if(CameraMain == nullptr) return;
	float GreatestDistanceBetweenTargets = CalculateGreatestDistanceBetweenTargets();

	/*float CurrentDistancePercent = FMath::GetMappedRangeValueClamped(
		FVector2D(CameraZoomDistanceBetweenTargetsMin, CameraZoomDistanceBetweenTargetsMax),
		FVector2D(0, 1),
		GreatestDistanceBetweenTargets);*/

	float CurrentDistancePercent = FMath::Clamp(UKismetMathLibrary::NormalizeToRange(GreatestDistanceBetweenTargets, GetDefault<UCameraSettings>()->DistanceBetweenTargetsMin, GetDefault<UCameraSettings>()->DistanceBetweenTargetsMax), 0, 1);

	float NewY = FMath::Lerp(CameraZoomYMin, CameraZoomYMax, CurrentDistancePercent);

	FVector LerpedZoom = FMath::Lerp(CameraMain->GetComponentLocation(), FVector(CameraMain->GetComponentLocation().X, NewY, CameraMain->GetComponentLocation().Z), GetDefault<UCameraSettings>()->SizeDampingFactor * DeltaTime);
	CameraMain->SetWorldLocation(LerpedZoom);
}

void UCameraWorldSubsystem::TickUpdateCameraPosition(float DeltaTime)
{
	if(CameraMain == nullptr) return;
	FVector AveragePosition = CalculateAveragePositionBetweenTargets();
	ClampPositionIntoCameraBounds(AveragePosition);

	FVector LerpedPos = FMath::Lerp(CameraMain->GetComponentLocation(), FVector(AveragePosition.X, CameraMain->GetComponentLocation().Y, AveragePosition.Z), GetDefault<UCameraSettings>()->PositionDampingFactor * DeltaTime);
	CameraMain->SetWorldLocation(LerpedPos);
}

FVector UCameraWorldSubsystem::CalculateAveragePositionBetweenTargets()
{
	float KeepY = CameraMain->GetComponentLocation().Y;
	FVector AveragePosition = FVector::ZeroVector;
	for(UObject* FollowTarget : FollowTargets) if (FollowTarget->Implements<UCameraFollowTarget>())
	{
		AveragePosition += Cast<ICameraFollowTarget>(FollowTarget)->GetFollowPosition();
	}
	AveragePosition /= FollowTargets.Num();
	return FVector(AveragePosition.X, KeepY, AveragePosition.Z);
}

UCameraComponent* UCameraWorldSubsystem::FindCameraByTag(const FName& Tag) const
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActors);
	for(AActor* Actor : FoundActors)
	{
		UCameraComponent* Camera = Actor->FindComponentByClass<UCameraComponent>();
		if(Camera != nullptr && Actor->ActorHasTag(Tag))
		{
			return Camera;
		}
	}
	return nullptr;
}

float UCameraWorldSubsystem::CalculateGreatestDistanceBetweenTargets()
{
	float GreatestDistance = 0.f;

	for(auto Target : FollowTargets)
	{
		for(auto OtherTarget : FollowTargets) if(Target != OtherTarget)
		{
			FVector TargetPosition = Cast<AActor>(Target)->GetActorLocation();
			FVector OtherTargetPosition = Cast<AActor>(OtherTarget)->GetActorLocation();
				
			float Distance = FVector::Distance(TargetPosition, OtherTargetPosition);
			if(Distance > GreatestDistance)
			{
				GreatestDistance = Distance;
			}
		}
	}

	return GreatestDistance;
}

void UCameraWorldSubsystem::InitCameraZoomParameters()
{

	TArray<AActor*> CameraDistanceActors;
	
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(GetDefault<UCameraSettings>()->CameraDistanceMinTag), CameraDistanceActors);
	auto CameraDistanceMin = CameraDistanceActors.Num() > 0 ? CameraDistanceActors[0] : nullptr;
	if(CameraDistanceMin)
	{
		CameraZoomYMin = CameraDistanceMin->GetActorLocation().Y;
	}

	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(GetDefault<UCameraSettings>()->CameraDistanceMaxTag), CameraDistanceActors);
	auto CameraDistanceMax = CameraDistanceActors.Num() > 0 ? CameraDistanceActors[0] : nullptr;
	if(CameraDistanceMax)
	{
		CameraZoomYMax = CameraDistanceMax->GetActorLocation().Y;
	}
}






