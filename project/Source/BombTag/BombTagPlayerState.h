#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BombTagPlayerState.generated.h"

UCLASS()
class BOMBTAG_API ABombTagPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    ABombTagPlayerState();

    // 닉네임 동기화 변수
    UPROPERTY(ReplicatedUsing = OnRep_PlayerNickname, BlueprintReadWrite, Category = "PlayerInfo")
    FString PlayerNickname;

    // 술래 여부 동기화 변수
    UPROPERTY(ReplicatedUsing = OnRep_IsBomber, BlueprintReadWrite, Category = "PlayerInfo")
    bool bIsBomber;

    // 값이 변경될 때 클라이언트에서 호출될 함수들
    UFUNCTION()
    void OnRep_PlayerNickname();

    UFUNCTION()
    void OnRep_IsBomber();

protected:
    // 네트워크 동기화 설정 (변수 등록)
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};