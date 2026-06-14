#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BombTagGameMode.generated.h"

UCLASS()
class BOMBTAG_API ABombTagGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABombTagGameMode();

protected:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// 라운드 타이머 업데이트
	void UpdateRoundTimer();

	// 라운드 종료 (폭탄 폭발)
	void HandleRoundEnd();

	// 살아남은 플레이어 중 무작위로 폭탄 할당
	void AssignRandomBomber();

public:
	// 한 라운드의 시간 (예: 15초)
	UPROPERTY(EditDefaultsOnly, Category = "Game Rules")
	int32 RoundTime;

	FTimerHandle RoundTimerHandle;
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_GameOver(const FString& WinnerName);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> ResultWidgetClass;
};