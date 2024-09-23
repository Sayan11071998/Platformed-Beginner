// Fill out your copyright notice in the Description page of Project Settings.


#include "Hook.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Components/PrimitiveComponent.h"

UHook::UHook()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHook::BeginPlay()
{
	Super::BeginPlay();

	GetComponentAndBind();
	
}

void UHook::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocation(GetReachEnd());
	}
}

void UHook::GetComponentAndBind()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();

	InputComponent->BindAction("Grab", IE_Pressed, this, &UHook::Grab);
	InputComponent->BindAction("Grab", IE_Released, this, &UHook::Release);
}

void UHook::Grab()
{
	FHitResult HitResult = GetPhysicsBodyInReach();
	auto GrabComponent = HitResult.GetComponent();
	auto ActorHit = HitResult.GetActor();

	if(ActorHit)
	{
		PhysicsHandle->GrabComponentAtLocation(GrabComponent, NAME_None, GrabComponent->GetOwner()->GetActorLocation());
	}
}

void UHook::Release()
{
	PhysicsHandle->ReleaseComponent();
}

FHitResult UHook::GetPhysicsBodyInReach()
{
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());

	FHitResult HitResult;

	GetWorld()->LineTraceSingleByObjectType(
		HitResult,
		GetReachStart(),
		GetReachEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);

	return HitResult;
}

FVector UHook::GetReachStart()
{
	FVector PlayerViewPoint;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerViewPoint, PlayerViewPointRotation);

	return PlayerViewPoint;
}

FVector UHook::GetReachEnd()
{
	FVector PlayerViewPoint;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerViewPoint, PlayerViewPointRotation);

	FVector LineTraceEnd = PlayerViewPoint + PlayerViewPointRotation.Vector() * Reach;

	return LineTraceEnd;
}