// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseWeapon.h"

#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "../Projectiles/BaseProjectile.h"

// Sets default values
ABaseWeapon::ABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	scene_root = CreateDefaultSubobject<USceneComponent>(TEXT("scene_root"));
	RootComponent = scene_root;

	weapon_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	weapon_mesh->SetupAttachment(scene_root);

	projectile_spawn_point = CreateDefaultSubobject<UArrowComponent>(TEXT("ProjectileSpawnPoint"));
	projectile_spawn_point->SetupAttachment(weapon_mesh);

	trigger_pulled = false;
	ability_to_fire = true;
}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseWeapon::spawn_proj()
{
	FActorSpawnParameters spawn_info;
	spawn_info.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FTransform transform = projectile_spawn_point->GetComponentTransform();
	transform.SetScale3D(FVector(1, 1, 1));
	GetWorld()->SpawnActor<ABaseProjectile>(weapon_config.projectile_type, transform, spawn_info);
}

void ABaseWeapon::fire()
{
	if (weapon_config.projectile_type) {
		spawn_proj();
		if (weapon_config.shot_sound) {
			UGameplayStatics::SpawnSoundAttached(weapon_config.shot_sound, projectile_spawn_point);
		}
	}
}

void ABaseWeapon::pull_trigger()
{
	if (!trigger_pulled && ability_to_fire)
	{
		trigger_pulled = true;

		fire();
		GetWorld()->GetTimerManager().SetTimer(timer_delay_bet_shots, this, &ABaseWeapon::fire, weapon_config.time_between_shots, true);
	}
}

void ABaseWeapon::release_trigger()
{
	//GetWorld()->GetTimerManager().ClearTimer(timer_delay_bet_shots);
	if (trigger_pulled)
	{
		ability_to_fire = false;
		GetWorld()->GetTimerManager().SetTimer(timer_delay_aft_shots, this, &ABaseWeapon::recover_fire_ability, weapon_config.time_between_shots, false);
	}
	trigger_pulled = false;
}

void ABaseWeapon::recover_fire_ability()
{
	GetWorld()->GetTimerManager().ClearTimer(timer_delay_aft_shots);
	ability_to_fire = true;
}
