#include "BombTagItemSpawner.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "EngineUtils.h"

ABombTagItemSpawner::ABombTagItemSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	// 스포너도 서버에서 관리되어야 하므로 리플리케이션 활성화
	bReplicates = true;
}

void ABombTagItemSpawner::BeginPlay()
{
	Super::BeginPlay();

	// 서버에서만 아이템 스폰 타이머를 가동
	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ABombTagItemSpawner::SpawnRandomItem, SpawnInterval, true);
	}
}

void ABombTagItemSpawner::SpawnRandomItem()
{
	if (!HasAuthority() || !ItemClass) return;

	// 1. 현재 맵에 존재하는 아이템 개수 세기
	int32 CurrentItemCount = 0;
	for (TActorIterator<ABombTagItem> It(GetWorld()); It; ++It)
	{
		CurrentItemCount++;
	}

	// 2. 최대 개수 제한 (예: 5개)
	int32 MaxItems = 5;
	if (CurrentItemCount >= MaxItems)
	{
		return; // 이미 아이템이 충분하면 생성 안 함
	}

	// 3. 아이템 생성
	FVector Origin = GetActorLocation();
	FVector RandomOffset = FMath::VRand() * FVector(SpawnRadius, SpawnRadius, 0.f);
	FVector SpawnLocation = Origin + RandomOffset;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;

	GetWorld()->SpawnActor<ABombTagItem>(ItemClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
}