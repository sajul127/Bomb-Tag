#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BombTagItem.h"
#include "BombTagItemSpawner.generated.h"

UCLASS()
class BOMBTAG_API ABombTagItemSpawner : public AActor
{
	GENERATED_BODY()

public:
	ABombTagItemSpawner();

protected:
	virtual void BeginPlay() override;

	// 생성할 아이템 클래스 (에디터에서 블루프린트로 지정)
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<ABombTagItem> ItemClass;

	// 아이템이 생성될 반경
	UPROPERTY(EditAnywhere, Category = "Spawning")
	float SpawnRadius = 1000.f;

	// 아이템을 주기적으로 스폰하기 위한 타이머 핸들
	FTimerHandle SpawnTimerHandle;

	// 아이템 스폰 간격
	UPROPERTY(EditAnywhere, Category = "Spawning")
	float SpawnInterval = 5.f;

public:
	// 아이템 생성 로직 함수
	void SpawnRandomItem();
};