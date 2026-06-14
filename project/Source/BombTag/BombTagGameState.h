#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BombTagGameState.generated.h"

/**
 * 게임의 전반적인 상태(남은 시간, 생존자 수 등)를 모든 클라이언트에게 동기화합니다.
 */
UCLASS()
class BOMBTAG_API ABombTagGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ABombTagGameState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 라운드 남은 시간 (UI 표시용)
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game State")
	int32 RemainingTime;

	// 현재 생존한 플레이어 수
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game State")
	int32 AlivePlayerCount;

	// 모두에게 알리는 함수
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_GameOver(const FString& WinnerName);

	// 블루프린트에서 UI를 띄우도록 연결해줄 이벤트 (C++ 구현부 없음!)
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void BP_ShowResultUI(const FString& WinnerName);
};