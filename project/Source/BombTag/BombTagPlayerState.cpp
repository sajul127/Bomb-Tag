#include "BombTagPlayerState.h"
#include "Net/UnrealNetwork.h" // 필수!

ABombTagPlayerState::ABombTagPlayerState()
{
    PlayerNickname = TEXT("Guest");
    bIsBomber = false;
}

// 동기화할 변수들을 등록하는 함수
void ABombTagPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ABombTagPlayerState, PlayerNickname);
    DOREPLIFETIME(ABombTagPlayerState, bIsBomber);
}

void ABombTagPlayerState::OnRep_PlayerNickname()
{
    // 닉네임이 변경되었을 때, 클라이언트에서 UI를 갱신할 코드를 여기에 작성합니다.
    UE_LOG(LogTemp, Log, TEXT("Nickname updated to: %s"), *PlayerNickname);
}

void ABombTagPlayerState::OnRep_IsBomber()
{
    // 술래 상태가 변경되었을 때, 캐릭터 머리 위 이펙트나 UI를 바꿀 코드를 여기에 작성합니다.
    UE_LOG(LogTemp, Log, TEXT("IsBomber status: %s"), bIsBomber ? TEXT("True") : TEXT("False"));
}