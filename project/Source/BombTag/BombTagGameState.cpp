#include "BombTagGameState.h"
#include "Net/UnrealNetwork.h"
#include "Blueprint/UserWidget.h"

ABombTagGameState::ABombTagGameState()
{
	RemainingTime = 0;
	AlivePlayerCount = 0;
}

void ABombTagGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 클라이언트들에게 변수 동기화
	DOREPLIFETIME(ABombTagGameState, RemainingTime);
	DOREPLIFETIME(ABombTagGameState, AlivePlayerCount);
}

void ABombTagGameState::Multicast_GameOver_Implementation(const FString& WinnerName)
{
	BP_ShowResultUI(WinnerName);
}