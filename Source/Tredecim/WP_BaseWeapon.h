// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine.h"
#include "WP_BaseWeapon.generated.h"

UCLASS(Blueprintable)
class TREDECIM_API AWP_BaseWeapon : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWP_BaseWeapon(const FObjectInitializer& ObjectInitializer);
	AWP_BaseWeapon();

	virtual void Tick(float DeltaSeconds) override;

	/*Owner*/
	UPROPERTY(BlueprintReadWrite, Category = Weapon)
		AActor* WeaponOwner;

	/* Checks if we have hit the target on this current swing*/
	UFUNCTION(BlueprintCallable, Category = Weapon)
		bool CheckHurtEnemies(ACharacter* HurtChar);

	UFUNCTION(BlueprintCallable, Category = Weapon)
		bool CheckHurtActors(AActor* HurtActor);

	/* holds all of the enemies hit on a swing*/
	UPROPERTY(BlueprintReadOnly, Category = Weapon)
		TArray<ACharacter*> EnemiesHitOnThisSwing;

	UPROPERTY(BlueprintReadOnly, Category = Weapon)
		TArray<AActor*> ActorsHitOnThisSwing;

	/* how many traces this weapon will do per swing*/
	UPROPERTY(EditAnywhere, Category = Trace)
		float TracesPerSwing;

	/* pawns within this radius are eligible for getting hit by the weapon*/
	UPROPERTY(EditAnywhere, Category = Trace)
		float MeleeRadius;

	/* Name of the socket to start the tracing from*/
	UPROPERTY(EditAnywhere, Category = Trace)
		FName SocketStartName;

	/*Whatever the melee trace can hit world actors too or not*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Trace)
		bool bTraceAgainstWorld;

	/* Name of the socket where the tracing ends*/
	UPROPERTY(EditAnywhere, Category = Trace)
		FName SocketEndName;

	/*Starts the tracing process*/
	UFUNCTION(BlueprintCallable, Category = Weapon)
		void BeginSwing();

	/*Ends the tracing process, empties the target list*/
	UFUNCTION(BlueprintCallable, Category = Weapon)
		void EndSwing();

	/*Called once this weapon has hit a pawn*/
	UFUNCTION(BlueprintImplementableEvent, Category = Weapon)
		void OnHit(FHitResult HitInfo);

	UFUNCTION(BlueprintImplementableEvent, Category = Weapon)
		void OnHitWorld(FHitResult HitInfoAct);

	void MeleeTraceWorld(FVector TraceStart, FVector TraceEnd);

	/*Whatever the weapon should trace or not*/
	UPROPERTY(BlueprintReadWrite, Category = Trace)
		bool bTrace;

	UPROPERTY(BlueprintReadWrite, Category = Trace)
		bool bLightAttack = true;

	/*If this is true we will use a line trace instead of a component trace ( it hits the character capsule instead )*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Trace)
		bool bUseLineTrace;

	/*Draws a debug line to show the tracing */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Debug)
		bool bDrawTraces;

	/*Holds the mesh of the weapon*/
	UPROPERTY(EditAnywhere, Category = Weapon)
		USkeletalMeshComponent* Mesh;

	/*These hold the location of the sockets on the previous frame*/
	FVector LastFrameStartSockEnd;
	FVector LastFrameEndSockEnd;

	/*The location of the sockets when the swing begun*/
	FVector StartSockEnd;
	FVector EndSockEnd;

	UPROPERTY()
		FName HitBone;

	UFUNCTION(BlueprintCallable, Category = Trace)
		FName GetLastHitBone();

	/*Damage Values for light and Heavy*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		float LightDamage = 25;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		float HeavyDamage = 40;

	/*Is this a Two Handed Weapon*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		bool bIsTwoHanded = false;

	/*Is this a Ranged Weapon*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		bool bIsRanged = false;
};
