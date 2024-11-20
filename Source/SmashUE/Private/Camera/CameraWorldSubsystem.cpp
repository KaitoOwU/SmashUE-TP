// Fill out your copyright notice in the Description page of Project Settings.

#include "Camera/CameraWorldSubsystem.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraFollowTarget.h"
#include "Kismet/GameplayStatics.h"

void UCameraWorldSubsystem::PostInitialize()
{
	Super::PostInitialize();
}

void UCameraWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	CameraMain = FindCameraByTag(TEXT("CameraMain"));

	AActor* CameraBoundsActor = FindCameraBoundsActor();
	if(CameraBoundsActor != nullptr)
	{
		InitCameraBounds(CameraBoundsActor);
	}
}

void UCameraWorldSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "CameraBounds", CameraBounds);
	return CameraBounds.Num() > 0 ? CameraBounds[0] : nullptr;
}

void UCameraWorldSubsystem::InitCameraBounds(AActor* CameraBoundsActor)
{
	FVector BoundsCenter;
	FVector BoundsExtents;
	CameraBoundsActor->GetActorBounds(false, BoundsCenter, BoundsExtents);

	CameraBoundsYProjectionCenter = BoundsCenter.Z;
	CameraBoundsMin = FVector2D(BoundsCenter.X - BoundsExtents.X, BoundsCenter.Z - BoundsExtents.Z);
	CameraBoundsMax = FVector2D(BoundsCenter.X + BoundsExtents.X, BoundsCenter.Z + BoundsExtents.Z);
}

void UCameraWorldSubsystem::ClampPositionIntoCameraBounds(FVector& Position)
{
	FVector2D ViewportBoundsMin,ViewportBoundsMax;
	GetViewportBounds(ViewportBoundsMin, ViewportBoundsMax);
	
	FVector WorldBoundsMin = CalculateWorldPositionFromViewportPosition(ViewportBoundsMin);
	FVector WorldBoundsMax = CalculateWorldPositionFromViewportPosition(ViewportBoundsMax);
	
	if(Position.X + WorldBoundsMin.X < CameraBoundsMin.X)
	{
		Position.X = CameraBoundsMin.X - WorldBoundsMin.X;
	} else if(Position.X + WorldBoundsMax.X >= CameraBoundsMax.X)
	{
		Position.X = CameraBoundsMax.X - WorldBoundsMax.X;
	}

	if(Position.Z + WorldBoundsMax.Z < CameraBoundsMin.Y)
	{
		Position.Z = CameraBoundsMin.Y - WorldBoundsMax.Z;
	} else if(Position.Z + WorldBoundsMin.Z >= CameraBoundsMax.Y)
	{
		Position.Z = CameraBoundsMax.Y - WorldBoundsMin.Z;
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

	float YDistanceToCenter = CameraMain->GetOwner()->GetActorLocation().Y - CameraBoundsYProjectionCenter;

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

void UCameraWorldSubsystem::TickUpdateCameraPosition(float DeltaTime)
{
	if(CameraMain == nullptr) return;
	FVector AveragePosition = CalculateAveragePositionBetweenTargets();
	ClampPositionIntoCameraBounds(AveragePosition);
	CameraMain->SetWorldLocation(AveragePosition);
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






